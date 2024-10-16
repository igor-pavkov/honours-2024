static int bsearchenv(char **env, const char *name, size_t size)
{
	unsigned low = 0, high = size;
	while (low < high) {
		unsigned mid = low + ((high - low) >> 1);
		int cmp = compareenv(&env[mid], &name);
		if (cmp < 0)
			low = mid + 1;
		else if (cmp > 0)
			high = mid;
		else
			return mid;
	}
	return ~low; /* not found, return 1's complement of insert position */
}