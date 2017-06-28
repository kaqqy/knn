#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "json.h"
#include "pq.h"

static void	knn_insert(json_value *data, json_value *idx_array, json_value *node_bounds, json_value *node_data, t_pq *pq, int index)
{
	// for some reason there's an extra array
	t_arr	*center = arr_create(node_bounds->u.array.values[0]->u.array.values[index], -1);
	double	radius = node_data->u.array.values[index]->u.array.values[3]->u.dbl;
	int		i;
	t_arr	*pt;

	if (pq->size == pq->max_size && arr_dist(center, pq->query) - radius > pq_max_dist(pq))
	{
		// priority queue is full, closest point on ball is further away from furthest point in priority queue
		return;
	}
	if (node_data->u.array.values[index]->u.array.values[2]->u.integer == 0)
	{
		// recursively call both children
		knn_insert(data, idx_array, node_bounds, node_data, pq, 2 * index + 1);
		knn_insert(data, idx_array, node_bounds, node_data, pq, 2 * index + 2);
	}
	else
	{
		// iterate through each node in leaf
		for (i = node_data->u.array.values[index]->u.array.values[0]->u.integer; i < node_data->u.array.values[index]->u.array.values[1]->u.integer; i++)
		{
			pt = arr_create(data->u.array.values[idx_array->u.array.values[i]->u.integer], i);
			pq_insert(pq, pt); // attempts to insert
		}
	}
}

static void	knn(json_value *data, json_value *idx_array, json_value *node_bounds, json_value *node_data, json_value *test_data, int neighbors)
{
	t_pq	*pq;
	t_arr	*query;
	int		i;

	query = arr_create(test_data, -1);
	pq = pq_create(neighbors, query);
	knn_insert(data, idx_array, node_bounds, node_data, pq, 0);
	pq_quicksort(pq, 0, pq->size - 1);
	for (i = 0; i < pq->size; i++)
	{
		printf("index: %d ; distance: %f\n", pq->arr[i]->index, arr_dist(query, pq->arr[i]));
	}
	printf("\n");
}

static int	use_json(json_value *value, json_value *test_data)
{
	int			i;
	int			length;
	json_value	*data = NULL;
	json_value	*idx_array = NULL;
	json_value	*node_bounds = NULL;
	json_value	*node_data = NULL;
	int			neighbors = -1;

	if (value->type != json_object)
	{
		fprintf(stderr, "Json is not an object\n");
		return 1;
	}
	length = value->u.object.length;
	for (i = 0; i < length; i++)
	{
		if (strcmp(value->u.object.values[i].name, "n_neighbors") == 0)
		{
			neighbors = value->u.object.values[i].value->u.integer;
			break;
		}
	}
	if (neighbors <= 0)
	{
		fprintf(stderr, "Invalid neighbors\n");
		return 1;
	}
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
	if (test_data == NULL)
	{
		test_data = data;
	}
	for (i = 0; i < test_data->u.array.length; i++)
	{
		knn(data, idx_array, node_bounds, node_data, test_data->u.array.values[i], neighbors);
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
	char		*file_contents2;
	json_value	*value2 = NULL;
	int			ret;

	if (argc < 2 || argc > 3)
	{
		fprintf(stderr, "%s <json_file> <optional_json_file>\n", argv[0]);
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
	if (fread(file_contents, file_size, 1, fp) != 1)
	{
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

	if (argc == 3)
	{
		// im lazy
		filename = argv[2];
		if (stat(filename, &filestatus) != 0)
			return 1;
		file_size = filestatus.st_size;
		file_contents2 = (char*)malloc(filestatus.st_size);
		fp = fopen(filename, "rt");
		fread(file_contents2, file_size, 1, fp);
		fclose(fp);
		value2 = json_parse((json_char*)file_contents2, file_size);
	}

	ret = use_json(value, value2); // everything that doesn't involve parsing the json file

	json_value_free(value);
	free(file_contents);
	return ret;
}
