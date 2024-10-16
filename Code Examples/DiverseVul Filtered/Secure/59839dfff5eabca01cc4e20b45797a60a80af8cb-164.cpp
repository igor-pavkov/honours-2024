static inline u32 bit(int bitno)
{
	return 1 << (bitno & 31);
}