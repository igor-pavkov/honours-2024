static int check_name(int argc, const char **argv, const char *prefix)
{
	if (argc > 1) {
		while (*++argv) {
			if (check_submodule_name(*argv) < 0)
				return 1;
		}
	} else {
		struct strbuf buf = STRBUF_INIT;
		while (strbuf_getline(&buf, stdin) != EOF) {
			if (!check_submodule_name(buf.buf))
				printf("%s\n", buf.buf);
		}
		strbuf_release(&buf);
	}
	return 0;
}