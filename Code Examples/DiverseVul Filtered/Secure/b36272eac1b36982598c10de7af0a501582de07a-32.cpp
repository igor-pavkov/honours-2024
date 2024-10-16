static void evp_cipher_decrypt(struct ssh_cipher_struct *cipher,
                               void *in,
                               void *out,
                               size_t len)
{
    int outlen = 0;
    int rc = 0;

    rc = EVP_DecryptUpdate(cipher->ctx,
                           (unsigned char *)out,
                           &outlen,
                           (unsigned char *)in,
                           (int)len);
    if (rc != 1){
        SSH_LOG(SSH_LOG_WARNING, "EVP_DecryptUpdate failed");
        return;
    }
    if (outlen != (int)len){
        SSH_LOG(SSH_LOG_WARNING,
                "EVP_DecryptUpdate: output size %d for %zu in",
                outlen,
                len);
        return;
    }
}