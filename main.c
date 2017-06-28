#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "json.h"

static void	use_json(json_value *value)
{
	if (value->type == json_object)
	{
		printf("is object\n");
	}
	else
	{
		printf("is not object\n");
	}
}

int			main(int argc, char **argv)
{
	char		*filename;
	FILE		*fp;
	struct stat	filestatus;
	int			file_size;
	char		*file_contents;
	json_char	*json;
	json_value	*value;

	if (argc != 2)
	{
		fprintf(stderr, "%s <file_json>\n", argv[0]);
		return 1;
	}
	filename = argv[1];

	if (stat(filename, &filestatus) != 0)
	{
		fprintf(stderr, "File %s not found\n", filename);
		return 1;
	}
	file_size = filestatus.st_size;
	file_contents = (char*)malloc(filestatus.st_size);
	if (file_contents == NULL)
	{
		fprintf(stderr, "Memory error: unable to allocate %d bytes\n", file_size);
		return 1;
	}

	fp = fopen(filename, "rt");
	if (fp == NULL)
	{
		fprintf(stderr, "Unable to open %s\n", filename);
		fclose(fp);
		free(file_contents);
		return 1;
	}
	int test;
	if ((test = fread(file_contents, file_size, 1, fp)) != 1)
	{
		fprintf(stderr, "%d\n", test);
		fprintf(stderr, "Unable to read content of %s\n", filename);
		fclose(fp);
		free(file_contents);
		return 1;
	}
	fclose(fp);

	printf("%s\n", file_contents);
	printf("------------------------------\n\n");

	json = (json_char*)file_contents;
	value = json_parse(json, file_size);
	if (value == NULL)
	{
		fprintf(stderr, "Unable to parse data\n");
		free(file_contents);
		exit(1);
	}

	use_json(value); // everything that doesn't involve parsing the json file

	json_value_free(value);
	free(file_contents);
	return 0;
}
