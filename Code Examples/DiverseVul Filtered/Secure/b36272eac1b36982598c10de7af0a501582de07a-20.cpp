void sha512(const unsigned char *digest, int len, unsigned char *hash)
{
    SHA512CTX c = sha512_init();
    if (c != NULL) {
        sha512_update(c, digest, len);
        sha512_final(hash, c);
    }
}