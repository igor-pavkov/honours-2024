static inline void *align_ptr(void *ptr)
{
	return (void *)align_val((size_t)ptr);
}