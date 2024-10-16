static bool complete_word(char ***result, char *start, char *end, size_t *cap, size_t *cnt)
{
	int r;

	r = lxc_grow_array((void ***)result, cap, 2 + *cnt, 16);
	if (r < 0)
		return false;

	(*result)[*cnt] = strndup(start, end - start);
	if (!(*result)[*cnt])
		return false;

	(*cnt)++;

	return true;
}