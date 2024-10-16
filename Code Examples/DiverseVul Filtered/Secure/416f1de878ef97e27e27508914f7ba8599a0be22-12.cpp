static void destroy_tag(char *tag[32])
{
	int i = 0;
	for (i = 0; i < 32; i++)
		free(tag[i]);
}