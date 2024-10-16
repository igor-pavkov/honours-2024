static int _r_aes_key_unwrap(uint8_t * kek, size_t kek_len, uint8_t * key, size_t key_len, uint8_t * wrapped_key) {
  struct aes128_ctx ctx_128;
  struct aes192_ctx ctx_192;
  struct aes256_ctx ctx_256;
  void * ctx = NULL;
  nettle_cipher_func * decrypt = NULL;
  const uint8_t default_iv[] = {0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6};

  if (kek_len == 16) {
    aes128_set_decrypt_key(&ctx_128, kek);
    ctx = (void*)&ctx_128;
    decrypt = (nettle_cipher_func*)&aes128_decrypt;
  }
  if (kek_len == 24) {
    aes192_set_decrypt_key(&ctx_192, kek);
    ctx = (void*)&ctx_192;
    decrypt = (nettle_cipher_func*)&aes192_decrypt;
  }
  if (kek_len == 32) {
    aes256_set_decrypt_key(&ctx_256, kek);
    ctx = (void*)&ctx_256;
    decrypt = (nettle_cipher_func*)&aes256_decrypt;
  }
  return nist_keyunwrap16(ctx, decrypt, default_iv, key_len, key, wrapped_key);
}