#include <ctype.h>
#include <errno.h>
#include <pcre.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>
#include <limits.h>
#include <selinux/selinux.h>

#include "../src/label_file.h"

static int validate_context(char __attribute__ ((unused)) **ctx)
{
	return 0;
}

static int process_file(struct selabel_handle *rec, const char *filename)
{
	unsigned int line_num;
	int rc;
	char *line_buf = NULL;
	size_t line_len = 0;
	FILE *context_file;
	const char *prefix = NULL;

	context_file = fopen(filename, "r");
	if (!context_file) {
		fprintf(stderr, "Error opening %s: %s\n",
			    filename, strerror(errno));
		return -1;
	}

	line_num = 0;
	rc = 0;
	while (getline(&line_buf, &line_len, context_file) > 0) {
		rc = process_line(rec, filename, prefix, line_buf, ++line_num);
		if (rc)
			goto out;
	}
out:
	free(line_buf);
	fclose(context_file);
	return rc;
}

/*
 * File Format
 *
 * u32 - magic number
 * u32 - version
 * u32 - length of pcre version EXCLUDING nul
 * char - pcre version string EXCLUDING nul
 * u32 - number of stems
 * ** Stems
 *	u32  - length of stem EXCLUDING nul
 *	char - stem char array INCLUDING nul
 * u32 - number of regexs
 * ** Regexes
 *	u32  - length of upcoming context INCLUDING nul
 *	char - char array of the raw context
 *	u32  - length of the upcoming regex_str
 *	char - char array of the original regex string including the stem.
 *	u32  - mode bits for >= SELINUX_COMPILED_FCONTEXT_MODE
 *	       mode_t for <= SELINUX_COMPILED_FCONTEXT_PCRE_VERS
 *	s32  - stemid associated with the regex
 *	u32  - spec has meta characters
 *	u32  - The specs prefix_len if >= SELINUX_COMPILED_FCONTEXT_PREFIX_LEN
 *	u32  - data length of the pcre regex
 *	char - a bufer holding the raw pcre regex info
 *	u32  - data length of the pcre regex study daya
 *	char - a buffer holding the raw pcre regex study data
 */
static int write_binary_file(struct saved_data *data, int fd)
{
	struct spec *specs = data->spec_arr;
	FILE *bin_file;
	size_t len;
	uint32_t magic = SELINUX_MAGIC_COMPILED_FCONTEXT;
	uint32_t section_len;
	uint32_t i;
	int rc;

	bin_file = fdopen(fd, "w");
	if (!bin_file) {
		perror("fopen output_file");
		exit(EXIT_FAILURE);
	}

	/* write some magic number */
	len = fwrite(&magic, sizeof(uint32_t), 1, bin_file);
	if (len != 1)
		goto err;

	/* write the version */
	section_len = SELINUX_COMPILED_FCONTEXT_MAX_VERS;
	len = fwrite(&section_len, sizeof(uint32_t), 1, bin_file);
	if (len != 1)
		goto err;

	/* write the pcre version */
	section_len = strlen(pcre_version());
	len = fwrite(&section_len, sizeof(uint32_t), 1, bin_file);
	if (len != 1)
		goto err;
	len = fwrite(pcre_version(), sizeof(char), section_len, bin_file);
	if (len != section_len)
		goto err;

	/* write the number of stems coming */
	section_len = data->num_stems;
	len = fwrite(&section_len, sizeof(uint32_t), 1, bin_file);
	if (len != 1)
		goto err;

	for (i = 0; i < section_len; i++) {
		char *stem = data->stem_arr[i].buf;
		uint32_t stem_len = data->stem_arr[i].len;

		/* write the strlen (aka no nul) */
		len = fwrite(&stem_len, sizeof(uint32_t), 1, bin_file);
		if (len != 1)
			goto err;

		/* include the nul in the file */
		stem_len += 1;
		len = fwrite(stem, sizeof(char), stem_len, bin_file);
		if (len != stem_len)
			goto err;
	}

	/* write the number of regexes coming */
	section_len = data->nspec;
	len = fwrite(&section_len, sizeof(uint32_t), 1, bin_file);
	if (len != 1)
		goto err;

	for (i = 0; i < section_len; i++) {
		char *context = specs[i].lr.ctx_raw;
		char *regex_str = specs[i].regex_str;
		mode_t mode = specs[i].mode;
		size_t prefix_len = specs[i].prefix_len;
		int32_t stem_id = specs[i].stem_id;
		pcre *re = specs[i].regex;
		pcre_extra *sd = get_pcre_extra(&specs[i]);
		uint32_t to_write;
		size_t size;

		/* length of the context string (including nul) */
		to_write = strlen(context) + 1;
		len = fwrite(&to_write, sizeof(uint32_t), 1, bin_file);
		if (len != 1)
			goto err;

		/* original context strin (including nul) */
		len = fwrite(context, sizeof(char), to_write, bin_file);
		if (len != to_write)
			goto err;

		/* length of the original regex string (including nul) */
		to_write = strlen(regex_str) + 1;
		len = fwrite(&to_write, sizeof(uint32_t), 1, bin_file);
		if (len != 1)
			goto err;

		/* original regex string */
		len = fwrite(regex_str, sizeof(char), to_write, bin_file);
		if (len != to_write)
			goto err;

		/* binary F_MODE bits */
		to_write = mode;
		len = fwrite(&to_write, sizeof(uint32_t), 1, bin_file);
		if (len != 1)
			goto err;

		/* stem for this regex (could be -1) */
		len = fwrite(&stem_id, sizeof(stem_id), 1, bin_file);
		if (len != 1)
			goto err;

		/* does this spec have a metaChar? */
		to_write = specs[i].hasMetaChars;
		len = fwrite(&to_write, sizeof(to_write), 1, bin_file);
		if (len != 1)
			goto err;

		/* For SELINUX_COMPILED_FCONTEXT_PREFIX_LEN */
		to_write = prefix_len;
		len = fwrite(&to_write, sizeof(to_write), 1, bin_file);
		if (len != 1)
			goto err;

		/* determine the size of the pcre data in bytes */
		rc = pcre_fullinfo(re, NULL, PCRE_INFO_SIZE, &size);
		if (rc < 0)
			goto err;

		/* write the number of bytes in the pcre data */
		to_write = size;
		len = fwrite(&to_write, sizeof(uint32_t), 1, bin_file);
		if (len != 1)
			goto err;

		/* write the actual pcre data as a char array */
		len = fwrite(re, 1, to_write, bin_file);
		if (len != to_write)
			goto err;

		/* determine the size of the pcre study info */
		rc = pcre_fullinfo(re, sd, PCRE_INFO_STUDYSIZE, &size);
		if (rc < 0)
			goto err;

		/* write the number of bytes in the pcre study data */
		to_write = size;
		len = fwrite(&to_write, sizeof(uint32_t), 1, bin_file);
		if (len != 1)
			goto err;

		/* write the actual pcre study data as a char array */
		len = fwrite(sd->study_data, 1, to_write, bin_file);
		if (len != to_write)
			goto err;
	}

	rc = 0;
out:
	fclose(bin_file);
	return rc;
err:
	rc = -1;
	goto out;
}

static void free_specs(struct saved_data *data)
{
	struct spec *specs = data->spec_arr;
	unsigned int num_entries = data->nspec;
	unsigned int i;

	for (i = 0; i < num_entries; i++) {
		free(specs[i].lr.ctx_raw);
		free(specs[i].lr.ctx_trans);
		free(specs[i].regex_str);
		free(specs[i].type_str);
		pcre_free(specs[i].regex);
		pcre_free_study(specs[i].sd);
	}
	free(specs);

	num_entries = data->num_stems;
	for (i = 0; i < num_entries; i++)
		free(data->stem_arr[i].buf);
	free(data->stem_arr);

	memset(data, 0, sizeof(*data));
}

static void usage(const char *progname)
{
	fprintf(stderr,
		"usage: %s [-o out_file] fc_file\n"
		"Where:\n\t"
		"-o      Optional file name of the PCRE formatted binary\n\t"
		"        file to be output. If not specified the default\n\t"
		"        will be fc_file with the .bin suffix appended.\n\t"
		"fc_file The text based file contexts file to be processed.\n",
		progname);
		exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	const char *path = NULL;
	const char *out_file = NULL;
	char stack_path[PATH_MAX + 1];
	char *tmp = NULL;
	int fd, rc, opt;
	struct stat buf;
	struct selabel_handle *rec = NULL;
	struct saved_data *data = NULL;

	if (argc < 2)
		usage(argv[0]);

	while ((opt = getopt(argc, argv, "o:")) > 0) {
		switch (opt) {
		case 'o':
			out_file = optarg;
			break;
		default:
			usage(argv[0]);
		}
	}

	if (optind >= argc)
		usage(argv[0]);

	path = argv[optind];
	if (stat(path, &buf) < 0) {
		fprintf(stderr, "Can not stat: %s: %m\n", path);
		exit(EXIT_FAILURE);
	}

	/* Generate dummy handle for process_line() function */
	rec = (struct selabel_handle *)calloc(1, sizeof(*rec));
	if (!rec) {
		fprintf(stderr, "Failed to calloc handle\n");
		exit(EXIT_FAILURE);
	}
	rec->backend = SELABEL_CTX_FILE;

	/* Need to set validation on to get the bin file generated by the
	 * process_line function, however as the bin file being generated
	 * may not be related to the currently loaded policy (that it
	 * would be validated against), then set callback to ignore any
	 * validation. */
	rec->validating = 1;
	selinux_set_callback(SELINUX_CB_VALIDATE,
			    (union selinux_callback)&validate_context);

	data = (struct saved_data *)calloc(1, sizeof(*data));
	if (!data) {
		fprintf(stderr, "Failed to calloc saved_data\n");
		free(rec);
		exit(EXIT_FAILURE);
	}

	rec->data = data;

	rc = process_file(rec, path);
	if (rc < 0)
		goto err;

	rc = sort_specs(data);
	if (rc)
		goto err;

	if (out_file)
		rc = snprintf(stack_path, sizeof(stack_path), "%s", out_file);
	else
		rc = snprintf(stack_path, sizeof(stack_path), "%s.bin", path);

	if (rc < 0 || rc >= (int)sizeof(stack_path))
		goto err;

	tmp = malloc(strlen(stack_path) + 7);
	if (!tmp)
		goto err;

	rc = sprintf(tmp, "%sXXXXXX", stack_path);
	if (rc < 0)
		goto err;

	fd  = mkstemp(tmp);
	if (fd < 0)
		goto err;

	rc = fchmod(fd, buf.st_mode);
	if (rc < 0) {
		perror("fchmod failed to set permission on compiled regexs");
		goto err_unlink;
	}

	rc = write_binary_file(data, fd);
	if (rc < 0)
		goto err_unlink;

	rc = rename(tmp, stack_path);
	if (rc < 0)
		goto err_unlink;

	rc = 0;
out:
	free_specs(data);
	free(rec);
	free(data);
	free(tmp);
	return rc;

err_unlink:
	unlink(tmp);
err:
	rc = -1;
	goto out;
}
