int looks_like_command_line_option(const char *str)
{
	return str && str[0] == '-';
}