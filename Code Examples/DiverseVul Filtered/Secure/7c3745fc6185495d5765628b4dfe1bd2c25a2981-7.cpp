static void strbuf_cleanup_path(struct strbuf *sb)
{
	const char *path = cleanup_path(sb->buf);
	if (path > sb->buf)
		strbuf_remove(sb, 0, path - sb->buf);
}