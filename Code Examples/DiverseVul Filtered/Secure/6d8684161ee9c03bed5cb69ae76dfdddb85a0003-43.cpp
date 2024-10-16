static void *malloc_startup(size_t size)
{
	void *result = malloc(size);
	if (!result)
		die_startup();
	return result;
}