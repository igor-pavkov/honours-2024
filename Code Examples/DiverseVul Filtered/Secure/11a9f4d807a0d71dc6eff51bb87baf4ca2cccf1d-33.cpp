static struct strbuf *get_pathname(void)
{
	static struct strbuf pathname_array[4] = {
		STRBUF_INIT, STRBUF_INIT, STRBUF_INIT, STRBUF_INIT
	};
	static int index;
	struct strbuf *sb = &pathname_array[index];
	index = (index + 1) % ARRAY_SIZE(pathname_array);
	strbuf_reset(sb);
	return sb;
}