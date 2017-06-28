#include <stdlib.h>
#include "pq.h"

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

int		pq_maxi(t_pq *pq)
{
	int		i;
	int		maxi = -1;
	double	curdist;
	double	maxdist = DBL_MIN;

	for (i = 0; i < pq->size; i++)
	{
		curdist = arr_dist(pq->arr[i], pq->query);
		if (curdist > maxdist)
		{
			maxdist = curdist;
			maxi = i;
		}
	}
	return maxi;
}

double	pq_max_dist(t_pq *pq)
{
	return arr_dist(pq->arr[pq_maxi(pq)], pq->query);
}

void	pq_insert(t_pq *pq, t_arr *el)
{
	int		maxi;

	if (pq->size < pq->max_size)
	{
		pq->arr[pq->size] = el;
		pq->size++;
		return;
	}
	maxi = pq_maxi(pq);
	if (arr_dist(el, pq->query) < arr_dist(pq->arr[maxi], pq->query))
	{
		// free pq->arr[maxi];
		pq->arr[maxi] = el;
	}
}

void	swap(void **a, void **b)
{
	void	*c;

	c = *a;
	*a = *b;
	*b = c;
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
