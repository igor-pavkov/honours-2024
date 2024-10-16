static void evp_cipher_encrypt(struct ssh_cipher_struct *cipher,
                               void *in,
                               void *out,
                               size_t len)
{
    int outlen = 0;
    int rc = 0;

    rc = EVP_EncryptUpdate(cipher->ctx,
                           (unsigned char *)out,
                           &outlen,
                           (unsigned char *)in,
                           (int)len);
    if (rc != 1){
        SSH_LOG(SSH_LOG_WARNING, "EVP_EncryptUpdate failed");
        return;
    }
    if (outlen != (int)len){
        SSH_LOG(SSH_LOG_WARNING,
                "EVP_EncryptUpdate: output size %d for %zu in",
                outlen,
                len);
        return;
    }
}