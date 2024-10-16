static inline int phar_validate_alias(const char *alias, int alias_len) /* {{{ */
{
	return !(memchr(alias, '/', alias_len) || memchr(alias, '\\', alias_len) || memchr(alias, ':', alias_len) ||
		memchr(alias, ';', alias_len) || memchr(alias, '\n', alias_len) || memchr(alias, '\r', alias_len));
}