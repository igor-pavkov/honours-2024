static inline unsigned short limit(const unsigned short v, const unsigned short u)
{
	return (v > u) ? u : v;
}