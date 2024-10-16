static string openssl_HMACsha256(const void *key, size_t key_len, const void *data, size_t data_len){
    std::shared_ptr<char> out(new char[32], [](char *ptr) { delete[] ptr; });
    unsigned int out_len;

#if defined(OPENSSL_VERSION_NUMBER) && (OPENSSL_VERSION_NUMBER > 0x10100000L)
    //openssl 1.1.0新增api，老版本api作废
    HMAC_CTX *ctx = HMAC_CTX_new();
    HMAC_CTX_reset(ctx);
    HMAC_Init_ex(ctx, key, (int)key_len, EVP_sha256(), NULL);
    HMAC_Update(ctx, (unsigned char*)data, data_len);
    HMAC_Final(ctx, (unsigned char *)out.get(), &out_len);
    HMAC_CTX_reset(ctx);
    HMAC_CTX_free(ctx);
#else
    HMAC_CTX ctx;
    HMAC_CTX_init(&ctx);
    HMAC_Init_ex(&ctx, key, key_len, EVP_sha256(), NULL);
    HMAC_Update(&ctx, (unsigned char*)data, data_len);
    HMAC_Final(&ctx, (unsigned char *)out.get(), &out_len);
    HMAC_CTX_cleanup(&ctx);
#endif //defined(OPENSSL_VERSION_NUMBER) && (OPENSSL_VERSION_NUMBER > 0x10100000L)
    return string(out.get(),out_len);
}