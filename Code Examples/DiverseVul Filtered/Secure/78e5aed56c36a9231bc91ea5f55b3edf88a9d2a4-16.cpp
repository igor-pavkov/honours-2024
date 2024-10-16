static int cbtls_new_session(UNUSED SSL *s, SSL_SESSION *sess)
{
	size_t size;
	char buffer[2 * MAX_SESSION_SIZE + 1];

	size = sess->session_id_length;
	if (size > MAX_SESSION_SIZE) size = MAX_SESSION_SIZE;

	fr_bin2hex(sess->session_id, buffer, size);

	DEBUG2("  SSL: adding session %s to cache", buffer);

	return 1;
}