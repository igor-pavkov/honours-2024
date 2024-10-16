static int read_yes_no_answer(void)
{
	char answer[1024];

	if (fgets(answer, sizeof(answer), stdin)) {
		size_t answer_len = strlen(answer);
		int got_full_line = 0, c;

		/* remove the newline */
		if (answer_len >= 2 && answer[answer_len-2] == '\r') {
			answer[answer_len-2] = '\0';
			got_full_line = 1;
		} else if (answer_len >= 1 && answer[answer_len-1] == '\n') {
			answer[answer_len-1] = '\0';
			got_full_line = 1;
		}
		/* flush the buffer in case we did not get the full line */
		if (!got_full_line)
			while ((c = getchar()) != EOF && c != '\n')
				;
	} else
		/* we could not read, return the
		 * default answer which is no */
		return 0;

	if (tolower(answer[0]) == 'y' && !answer[1])
		return 1;
	if (!strncasecmp(answer, "yes", sizeof(answer)))
		return 1;
	if (tolower(answer[0]) == 'n' && !answer[1])
		return 0;
	if (!strncasecmp(answer, "no", sizeof(answer)))
		return 0;

	/* did not find an answer we understand */
	return -1;
}