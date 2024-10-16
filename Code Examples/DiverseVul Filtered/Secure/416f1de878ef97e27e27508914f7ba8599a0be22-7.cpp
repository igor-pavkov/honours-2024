static int setup_tag(char *tag[32], char *line)
{
	int i = 0;
	char *context;

	tag[0] = strtok_r(line, "\t \r\n", &context);

	while (tag[i] && i < 31) {
		tag[++i] = strtok_r(NULL, "\t \r\n", &context);
	}
	while (i < 32) {
		tag[i++] = "";
	}

	if (!tag[0][0]) {
		return 0;
	}

	for (i = 0; i < 32; i++) {
		tag[i] = strdup(url_decode(tag[i]));
	}

	return 1;
}