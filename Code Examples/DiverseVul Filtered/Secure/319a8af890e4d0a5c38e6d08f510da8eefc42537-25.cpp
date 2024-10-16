gchar *utf8_strncpy(gchar *dst, const gchar *src, gsize byte_len)
{
	/* -1 for '\0' in buffer */
	glong char_len = g_utf8_strlen(src, byte_len - 1);
	return g_utf8_strncpy(dst, src, char_len);
}