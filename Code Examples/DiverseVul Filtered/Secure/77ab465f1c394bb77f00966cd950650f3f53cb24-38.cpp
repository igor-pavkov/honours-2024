void js_stacktrace(js_State *J)
{
	int n;
	printf("stack trace:\n");
	for (n = J->tracetop; n >= 0; --n) {
		const char *name = J->trace[n].name;
		const char *file = J->trace[n].file;
		int line = J->trace[n].line;
		if (line > 0) {
			if (name[0])
				printf("\tat %s (%s:%d)\n", name, file, line);
			else
				printf("\tat %s:%d\n", file, line);
		} else
			printf("\tat %s (%s)\n", name, file);
	}
}