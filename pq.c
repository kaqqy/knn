#include <stdlib.h>
#include "pq.h"

static void	swap(void **a, void **b)
{
	void	*c;

	c = *a;
	*a = *b;
	*b = c;
}

t_arr	*arr_create(json_value *jarr, int index)
{
	t_arr	*arr;
	int		i;

	arr = (t_arr*)malloc(sizeof(*arr));
	if (!arr)
		return NULL;
	arr->size = jarr->u.array.length;
	arr->num = (double*)malloc(sizeof(*(arr->num)) * arr->size);
	if (!arr->num)
	{
		free(arr);
		return NULL;
	}
	for (i = 0; i < arr->size; i++)
	{
		arr->num[i] = jarr->u.array.values[i]->u.dbl;
	}
	arr->index = index;
	return arr;
}

double	arr_dist(t_arr *a, t_arr *b)
{
	int		i;
	double	dist = 0;

	for (i = 0; i < a->size; i++)
	{
		if (a->num[i] < b->num[i])
		{
			dist += b->num[i] - a->num[i];
		}
		else
		{
			dist += a->num[i] - b->num[i];
		}
	}
	return dist;
}

t_pq	*pq_create(int k, t_arr *query)
{
	t_pq	*pq;
	int		i;

	if (k < 1)
		return NULL;
	pq = (t_pq*)malloc(sizeof(*pq));
	if (!pq)
		return NULL;
	pq->size = 0;
	pq->max_size = k;
	pq->arr = (t_arr**)malloc(sizeof(*(pq->arr)) * k);
	if (!pq->arr)
	{
		free(pq);
		return NULL;
	}
	for (i = 0; i < k; i++)
	{
		pq->arr[i] = NULL;
	}
	pq->query = query;
	return pq;
}

double	pq_max_dist(t_pq *pq)
{
	return arr_dist(pq->arr[0], pq->query);
}

void	heapify_up(t_pq *pq, int index)
{
	while (index > 0)
	{
		if (arr_dist(pq->query, pq->arr[index]) > arr_dist(pq->query, pq->arr[(index - 1) / 2]))
		{
			swap((void**)&pq->arr[index], (void**)&pq->arr[(index - 1) / 2]);
			index = (index - 1) / 2;
		}
		else
		{
			break;
		}
	}
}

void	heapify_down(t_pq *pq, int index)
{
	int		max_index = index;
	double	max_dist = arr_dist(pq->query, pq->arr[index]);
	double	cur_dist;

	if (2 * index + 1 < pq->size && (cur_dist = arr_dist(pq->query, pq->arr[2 * index + 1])) > max_dist)
	{
		max_index = 2 * index + 1;
		max_dist = cur_dist;
	}
	if (2 * index + 2 < pq->size && (cur_dist = arr_dist(pq->query, pq->arr[2 * index + 2])) > max_dist)
	{
		max_index = 2 * index + 2;
		max_dist = cur_dist;
	}
	if (max_index != index)
	{
		swap((void**)&pq->arr[index], (void**)&pq->arr[max_index]);
		heapify_down(pq, max_index);
	}
}

void	pq_insert(t_pq *pq, t_arr *el)
{
	int		maxi;

	if (pq->size < pq->max_size)
	{
		pq->arr[pq->size] = el;
		pq->size++;
		heapify_up(pq, pq->size - 1);
		return;
	}
	if (arr_dist(el, pq->query) < arr_dist(pq->arr[0], pq->query))
	{
		// free pq->arr[0];
		pq->arr[0] = el;
		heapify_down(pq, 0);
	}
}

void	pq_quicksort(t_pq *pq, int l, int r)
{
	int		p;
	int		i;

	if (l >= r)
	{
		return;
	}
	p = l;
	for (i = l; i < r; i++)
	{
		// pivot is at index r
		if (arr_dist(pq->query, pq->arr[i]) <= arr_dist(pq->query, pq->arr[r]))
		{
			swap((void**)&pq->arr[p], (void**)&pq->arr[i]);
			p++;
		}
	}
	swap((void**)&pq->arr[p], (void**)&pq->arr[r]);
	pq_quicksort(pq, l, p - 1);
	pq_quicksort(pq, p + 1, r);
}
