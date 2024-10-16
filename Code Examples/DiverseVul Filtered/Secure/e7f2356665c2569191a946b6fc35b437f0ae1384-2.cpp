static int strlen16 (unsigned short *s)
{
	int len = 0;
	while (*s) {
		s++;
		len++;
	}
	return len;
}