void sha1(const unsigned char *digest, int len, unsigned char *hash)
{
    SHACTX c = sha1_init();
    if (c != NULL) {
        sha1_update(c, digest, len);
        sha1_final(hash, c);
    }
}