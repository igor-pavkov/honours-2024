static void rnd_nonce_func(void *_ctx, size_t length, uint8_t * data)
{
  (void)_ctx;
	gnutls_rnd(GNUTLS_RND_NONCE, data, length);
}