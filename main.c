#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "json.h"
#include "pq.h"

static int	use_json(json_value *value)
{
	int			i;
	int			length;
	json_value	*data = NULL;
	json_value	*idx_array = NULL;
	json_value	*node_bounds = NULL;
	json_value	*node_data = NULL;

	if (value->type != json_object)
	{
		fprintf(stderr, "Json is not an object\n");
		return 1;
	}
	length = value->u.object.length;
	for (i = 0; i < length; i++)
	{
		if (strcmp(value->u.object.values[i].name, "_tree") == 0)
		{
			break;
		}
	}
	if (i == length)
	{
		fprintf(stderr, "key=_tree not found\n");
		return 1;
	}
	value = value->u.object.values[i].value;
	if (value->type != json_object)
	{
		fprintf(stderr, "_tree is not an object\n");
		return 1;
	}
	length = value->u.object.length;
	for (i = 0; i < length; i++)
	{
		if (strcmp(value->u.object.values[i].name, "data") == 0)
		{
			data = value->u.object.values[i].value;
		}
		else if (strcmp(value->u.object.values[i].name, "idx_array") == 0)
		{
			idx_array = value->u.object.values[i].value;
		}
		else if (strcmp(value->u.object.values[i].name, "node_bounds") == 0)
		{
			node_bounds = value->u.object.values[i].value;
		}
		else if (strcmp(value->u.object.values[i].name, "node_data") == 0)
		{
			node_data = value->u.object.values[i].value;
		}
	}
	if (data == NULL || idx_array == NULL || node_bounds == NULL || node_data == NULL)
	{
		fprintf(stderr, "Missing key in _tree\n");
		return 1;
	}

	return 0;
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
	int			ret;

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

	// printf("%s\n", file_contents);
	// printf("------------------------------\n\n");

	json = (json_char*)file_contents;
	value = json_parse(json, file_size);
	if (value == NULL)
	{
		fprintf(stderr, "Unable to parse data\n");
		free(file_contents);
		exit(1);
	}

	ret = use_json(value); // everything that doesn't involve parsing the json file

	json_value_free(value);
	free(file_contents);
	return ret;
}
