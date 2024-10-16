static long timelib_parse_tz_cor(char **ptr)
{
	char *begin = *ptr, *end;
	long  tmp;

	while (isdigit(**ptr) || **ptr == ':') {
		++*ptr;
	}
	end = *ptr;
	switch (end - begin) {
		case 1:
		case 2:
			return HOUR(strtol(begin, NULL, 10));
			break;
		case 3:
		case 4:
			if (begin[1] == ':') {
				tmp = HOUR(strtol(begin, NULL, 10)) + strtol(begin + 2, NULL, 10);
				return tmp;
			} else if (begin[2] == ':') {
				tmp = HOUR(strtol(begin, NULL, 10)) + strtol(begin + 3, NULL, 10);
				return tmp;
			} else {
				tmp = strtol(begin, NULL, 10);
				return HOUR(tmp / 100) + tmp % 100;
			}
		case 5:
			tmp = HOUR(strtol(begin, NULL, 10)) + strtol(begin + 3, NULL, 10);
			return tmp;
	}
	return 0;
}