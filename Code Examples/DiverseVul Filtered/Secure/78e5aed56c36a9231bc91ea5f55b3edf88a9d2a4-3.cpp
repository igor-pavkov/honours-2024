static SSL_SESSION *cbtls_get_session(UNUSED SSL *s,
				      unsigned char *data, int len,
				      UNUSED int *copy)
{
	size_t size;
	char buffer[2 * MAX_SESSION_SIZE + 1];

	size = len;
	if (size > MAX_SESSION_SIZE) size = MAX_SESSION_SIZE;

	fr_bin2hex(data, buffer, size);

        DEBUG2("  SSL: Client requested nonexistent cached session %s",
	       buffer);

	return NULL;
}