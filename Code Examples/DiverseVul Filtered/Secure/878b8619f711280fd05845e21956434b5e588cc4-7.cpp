static inline int atedge(const int p, int size_row)
{
	return (!(p % size_row)	|| !((p + 2) % size_row));
}