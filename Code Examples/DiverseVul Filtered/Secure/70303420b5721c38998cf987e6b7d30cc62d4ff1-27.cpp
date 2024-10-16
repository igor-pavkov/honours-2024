static bool is_not(const char *str)
{
	switch (str[1]) {
	case '=':
	case '~':
		return false;
	}
	return true;
}