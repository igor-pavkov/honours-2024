static void snippet_copy(const char *src, string_t *dst)
{
	while (*src != '\0' && i_isspace(*src)) src++;
	str_append(dst, src);
}