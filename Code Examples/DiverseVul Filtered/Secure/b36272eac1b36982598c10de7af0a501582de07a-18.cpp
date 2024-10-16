void sha384(const unsigned char *digest, int len, unsigned char *hash)
{
    SHA384CTX c = sha384_init();
    if (c != NULL) {
        sha384_update(c, digest, len);
        sha384_final(hash, c);
    }
}