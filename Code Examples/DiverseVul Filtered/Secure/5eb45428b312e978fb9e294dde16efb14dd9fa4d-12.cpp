static int lxc_append_null_to_list(void ***list)
{
	int newentry = 0;
	void **tmp;

	if (*list)
		for (; (*list)[newentry]; newentry++) {
			;
		}

	tmp = realloc(*list, (newentry + 2) * sizeof(void **));
	if (!tmp)
		return -1;

	*list = tmp;
	(*list)[newentry + 1] = NULL;

	return newentry;
}