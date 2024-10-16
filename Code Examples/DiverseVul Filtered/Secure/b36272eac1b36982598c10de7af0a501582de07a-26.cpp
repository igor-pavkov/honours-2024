HMACCTX hmac_init(const void *key, int len, enum ssh_hmac_e type) {
  HMACCTX ctx = NULL;

  ctx = HMAC_CTX_new();
  if (ctx == NULL) {
    return NULL;
  }


  switch(type) {
    case SSH_HMAC_SHA1:
      HMAC_Init_ex(ctx, key, len, EVP_sha1(), NULL);
      break;
    case SSH_HMAC_SHA256:
      HMAC_Init_ex(ctx, key, len, EVP_sha256(), NULL);
      break;
    case SSH_HMAC_SHA512:
      HMAC_Init_ex(ctx, key, len, EVP_sha512(), NULL);
      break;
    case SSH_HMAC_MD5:
      HMAC_Init_ex(ctx, key, len, EVP_md5(), NULL);
      break;
    default:
      HMAC_CTX_free(ctx);
      ctx = NULL;
  }

  return ctx;
}