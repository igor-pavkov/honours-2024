void utf8_snprintf(gchar *dst, gsize byte_len, const gchar *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	gchar *str = g_strdup_vprintf(fmt, ap); 
	va_end(ap);
	utf8_strncpy(dst, str, byte_len);
	g_free(str);
}