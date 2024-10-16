static inline size_t align_val(size_t val)
{
	return (val + ALIGN_MASK) & ~ALIGN_MASK;
}