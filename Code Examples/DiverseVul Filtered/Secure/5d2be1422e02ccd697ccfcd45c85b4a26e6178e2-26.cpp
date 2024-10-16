static inline u32 perc(u32 count, u32 total)
{
	return (count * 100 + (total / 2)) / total;
}