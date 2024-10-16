static int is_pid(const char *str)
{
	int ch;

	for (; (ch = *str); str++) {
		if (!isdigit(ch))
			return 0;
	}
	return 1;
}