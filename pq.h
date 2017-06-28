#ifndef PQ_H
#define PQ_H
#include <float.h>
#include "json.h"
typedef struct	s_arr
{
	int		size;
	double	*num;
	int		index;
}				t_arr;

typedef struct	s_pq
{
	int		size;
	int		max_size;
	t_arr	**arr;
	t_arr	*query;
}				t_pq;

t_arr	*arr_create(json_value *jarr, int index);
double	arr_dist(t_arr *a, t_arr *b);
t_pq	*pq_create(int k, t_arr *query);
int		pq_maxi(t_pq *pq);
double	pq_max_dist(t_pq *pq);
void	pq_insert(t_pq *pq, t_arr *el);
#endif
