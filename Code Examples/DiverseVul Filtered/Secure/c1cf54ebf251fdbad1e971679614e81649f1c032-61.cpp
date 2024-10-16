int print_to_file(const char *file, const char *content)
{
	FILE *f;
	int ret = 0;

	f = fopen(file, "w");
	if (!f)
		return -1;

	if (fprintf(f, "%s", content) != strlen(content))
		ret = -1;

	fclose(f);
	return ret;
}