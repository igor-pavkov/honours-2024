static char *web_server_asctime_r(const struct tm *tm, char *buf)
{
	if (tm == NULL || buf == NULL)
		return NULL;

	asctime_s(buf, ASCTIME_R_BUFFER_SIZE, tm);
	return buf;
}