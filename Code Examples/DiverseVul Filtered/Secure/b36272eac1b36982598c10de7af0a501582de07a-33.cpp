static const EVP_MD *sshkdf_digest_to_md(enum ssh_kdf_digest digest_type)
{
    switch (digest_type) {
    case SSH_KDF_SHA1:
        return EVP_sha1();
    case SSH_KDF_SHA256:
        return EVP_sha256();
    case SSH_KDF_SHA384:
        return EVP_sha384();
    case SSH_KDF_SHA512:
        return EVP_sha512();
    }
    return NULL;
}