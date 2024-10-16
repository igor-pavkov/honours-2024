static bool name_is_in_groupnames(char *name, char **groupnames)
{
	while (groupnames) {
		if (!strcmp(name, *groupnames))
			return true;
		groupnames++;
	}
	return false;
}