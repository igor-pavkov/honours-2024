static int set_password(struct parsed_mount_info *parsed_info, const char *src)
{
	char *dst = parsed_info->password;
	unsigned int i = 0, j = 0;

	while (src[i]) {
		if (src[i] == ',')
			dst[j++] = ',';
		dst[j++] = src[i++];
		if (j > sizeof(parsed_info->password)) {
			fprintf(stderr, "Converted password too long!\n");
			return EX_USAGE;
		}
	}
	dst[j] = '\0';
	parsed_info->got_password = 1;
	return 0;
}