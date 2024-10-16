static int starts_with_dot_dot_slash(const char *str)
{
	return str[0] == '.' && str[1] == '.' && is_dir_sep(str[2]);
}