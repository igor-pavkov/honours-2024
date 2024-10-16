static bool name_is_in_list(const char *name, char **list)
{
	uint32_t count;

	for (count = 0; list && list[count]; count++) {
		if (strequal(name, list[count])) {
			return true;
		}
	}
	return false;
}