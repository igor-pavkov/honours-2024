static inline int kvmppc_get_vsr_dword_offset(int index)
{
	int offset;

	if ((index != 0) && (index != 1))
		return -1;

#ifdef __BIG_ENDIAN
	offset =  index;
#else
	offset = 1 - index;
#endif

	return offset;
}