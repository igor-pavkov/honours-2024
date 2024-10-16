static int starts_with_dot_slash(const char *str)
{
	return str[0] == '.' && is_dir_sep(str[1]);
}