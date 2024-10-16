static inline int desc_empty(const void *ptr)
{
	const u32 *desc = ptr;
	return !(desc[0] | desc[1]);
}