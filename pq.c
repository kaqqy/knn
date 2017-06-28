#include <stdlib.h>
#include "pq.h"
#include "json.h"

t_arr	*arr_create(int k)
{
	t_arr	*arr;
	int		i;

	if (k < 1)
		return NULL;
	arr = (t_arr*)malloc(sizeof(*arr));
	if (!arr)
		return NULL;
	arr->size = k;
	arr->num = (double*)malloc(sizeof(*(arr->num)) * k);
	if (!arr->num)
	{
		free(arr);
		return NULL;
	}
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

t_pq	*pq_create(int k)
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
	return pq;
}

void	pq_insert(t_pq *pq, t_arr *el, t_arr *center)
{
	int		i;
	int		maxi = -1;
	double	curdist;
	double	maxdist = arr_dist(el, center);

	if (pq->size < pq->max_size)
	{
		pq->arr[pq->size] = el;
		pq->size++;
		return;
	}
	for (i = 0; i < pq->size; i++)
	{
		curdist = arr_dist(pq->arr[i], center);
		if (curdist > maxdist)
		{
			maxdist = curdist;
			maxi = i;
		}
	}
	if (maxi != -1)
	{
		// free pq->arr[maxi]
		pq->arr[maxi] = el;
	}
}
