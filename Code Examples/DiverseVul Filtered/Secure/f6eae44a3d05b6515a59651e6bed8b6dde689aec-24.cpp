static int uppercase_string(char *string)
{
	if (!string)
		return 1;

	while (*string) {
		/* check for unicode */
		if ((unsigned char)string[0] & 0x80)
			return 0;
		*string = toupper((unsigned char)*string);
		string++;
	}

	return 1;
}