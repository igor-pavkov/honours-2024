static int push_signature(uint8 **outbuf)
{
	char *lanman;
	int result, tmp;

	result = 0;

	tmp = message_push_string(outbuf, "Unix", STR_TERMINATE);

	if (tmp == -1) return -1;
	result += tmp;

	if (asprintf(&lanman, "Samba %s", samba_version_string()) != -1) {
		tmp = message_push_string(outbuf, lanman, STR_TERMINATE);
		SAFE_FREE(lanman);
	}
	else {
		tmp = message_push_string(outbuf, "Samba", STR_TERMINATE);
	}

	if (tmp == -1) return -1;
	result += tmp;

	tmp = message_push_string(outbuf, lp_workgroup(), STR_TERMINATE);

	if (tmp == -1) return -1;
	result += tmp;

	return result;
}