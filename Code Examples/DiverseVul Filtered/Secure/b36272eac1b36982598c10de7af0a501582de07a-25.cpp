int ssh_kdf(struct ssh_crypto_struct *crypto,
            unsigned char *key, size_t key_len,
            int key_type, unsigned char *output,
            size_t requested_len)
{
    EVP_KDF_CTX *ctx = EVP_KDF_CTX_new_id(EVP_KDF_SSHKDF);
    int rc;

    if (ctx == NULL) {
        return -1;
    }

    rc = EVP_KDF_ctrl(ctx, EVP_KDF_CTRL_SET_MD,
                      sshkdf_digest_to_md(crypto->digest_type));
    if (rc != 1) {
        goto out;
    }
    rc = EVP_KDF_ctrl(ctx, EVP_KDF_CTRL_SET_KEY, key, key_len);
    if (rc != 1) {
        goto out;
    }
    rc = EVP_KDF_ctrl(ctx, EVP_KDF_CTRL_SET_SSHKDF_XCGHASH,
                      crypto->secret_hash, crypto->digest_len);
    if (rc != 1) {
        goto out;
    }
    rc = EVP_KDF_ctrl(ctx, EVP_KDF_CTRL_SET_SSHKDF_TYPE, key_type);
    if (rc != 1) {
        goto out;
    }
    rc = EVP_KDF_ctrl(ctx, EVP_KDF_CTRL_SET_SSHKDF_SESSION_ID,
                      crypto->session_id, crypto->digest_len);
    if (rc != 1) {
        goto out;
    }
    rc = EVP_KDF_derive(ctx, output, requested_len);
    if (rc != 1) {
        goto out;
    }

out:
    EVP_KDF_CTX_free(ctx);
    if (rc < 0) {
        return rc;
    }
    return 0;
}