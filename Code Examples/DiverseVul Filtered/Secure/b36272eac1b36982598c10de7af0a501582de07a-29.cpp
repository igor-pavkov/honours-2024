void sha256(const unsigned char *digest, int len, unsigned char *hash)
{
    SHA256CTX c = sha256_init();
    if (c != NULL) {
        sha256_update(c, digest, len);
        sha256_final(hash, c);
    }
}