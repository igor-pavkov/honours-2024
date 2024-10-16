static void _r_aes_key_wrap(uint8_t * kek, size_t kek_len, uint8_t * key, size_t key_len, uint8_t * wrapped_key) {
  struct aes128_ctx ctx_128;
  struct aes192_ctx ctx_192;
  struct aes256_ctx ctx_256;
  void * ctx = NULL;
  nettle_cipher_func * encrypt = NULL;
  const uint8_t default_iv[] = {0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6};

  if (kek_len == 16) {
    aes128_set_encrypt_key(&ctx_128, kek);
    ctx = (void*)&ctx_128;
    encrypt = (nettle_cipher_func*)&aes128_encrypt;
  }
  if (kek_len == 24) {
    aes192_set_encrypt_key(&ctx_192, kek);
    ctx = (void*)&ctx_192;
    encrypt = (nettle_cipher_func*)&aes192_encrypt;
  }
  if (kek_len == 32) {
    aes256_set_encrypt_key(&ctx_256, kek);
    ctx = (void*)&ctx_256;
    encrypt = (nettle_cipher_func*)&aes256_encrypt;
  }
  nist_keywrap16(ctx, encrypt, default_iv, key_len+8, wrapped_key, key);
}