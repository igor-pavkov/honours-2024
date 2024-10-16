void hmac_final(HMACCTX ctx, unsigned char *hashmacbuf, unsigned int *len) {
  HMAC_Final(ctx,hashmacbuf,len);

#if OPENSSL_VERSION_NUMBER > 0x10100000L
  HMAC_CTX_free(ctx);
  ctx = NULL;
#else
  HMAC_cleanup(ctx);
  SAFE_FREE(ctx);
  ctx = NULL;
#endif
}