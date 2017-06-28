#ifndef PQ_H
#define PQ_H
typedef struct	s_arr
{
	int		size;
	double	*num;
}				t_arr;

typedef struct	s_pq
{
	int		size;
	int		max_size;
	t_arr	**arr;
}				t_pq;

#endif
