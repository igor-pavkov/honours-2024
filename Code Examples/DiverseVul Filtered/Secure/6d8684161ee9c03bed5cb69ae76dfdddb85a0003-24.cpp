static int ask_yes_no_if_possible(const char *format, ...)
{
	char question[4096];
	const char *retry_hook[] = { NULL, NULL, NULL };
	va_list args;

	va_start(args, format);
	vsnprintf(question, sizeof(question), format, args);
	va_end(args);

	if ((retry_hook[0] = mingw_getenv("GIT_ASK_YESNO"))) {
		retry_hook[1] = question;
		return !run_command_v_opt(retry_hook, 0);
	}

	if (!isatty(_fileno(stdin)) || !isatty(_fileno(stderr)))
		return 0;

	while (1) {
		int answer;
		fprintf(stderr, "%s (y/n) ", question);

		if ((answer = read_yes_no_answer()) >= 0)
			return answer;

		fprintf(stderr, "Sorry, I did not understand your answer. "
				"Please type 'y' or 'n'\n");
	}
}