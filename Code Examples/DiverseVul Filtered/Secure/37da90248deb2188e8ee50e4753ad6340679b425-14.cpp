ZEND_API void zend_freedtoa(char *s)
{
	Bigint *b = (Bigint *)((int *)s - 1);
	b->maxwds = 1 << (b->k = *(int*)b);
	Bfree(b);
}