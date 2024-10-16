static void cbtls_remove_session(UNUSED SSL_CTX *ctx, SSL_SESSION *sess)
{
	size_t size;
	VALUE_PAIR *vp;
	char buffer[2 * MAX_SESSION_SIZE + 1];

	size = sess->session_id_length;
	if (size > MAX_SESSION_SIZE) size = MAX_SESSION_SIZE;

	fr_bin2hex(sess->session_id, buffer, size);

        DEBUG2("  SSL: Removing session %s from the cache", buffer);
        SSL_SESSION_free(sess);

        return;
}