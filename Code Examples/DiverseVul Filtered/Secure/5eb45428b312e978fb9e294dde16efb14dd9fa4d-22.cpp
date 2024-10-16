int lxc_append_string(char ***list, char *entry)
{
	char *copy;
	int newentry;

	newentry = lxc_append_null_to_list((void ***)list);
	if (newentry < 0)
		return -1;

	copy = strdup(entry);
	if (!copy)
		return -1;

	(*list)[newentry] = copy;

	return 0;
}