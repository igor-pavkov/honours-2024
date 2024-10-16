static inline unsigned int get32(char *ptr)
{
	unsigned char *tmp = (unsigned char*) ptr;

	return tmp[0] | (tmp[1] << 8) | (tmp[2] << 16) | (tmp[3] << 24);
}