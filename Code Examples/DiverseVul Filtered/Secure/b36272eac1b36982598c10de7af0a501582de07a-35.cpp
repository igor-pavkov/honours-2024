void sha512_final(unsigned char *md, SHA512CTX c)
{
    unsigned int mdlen = 0;

    EVP_DigestFinal(c, md, &mdlen);
    EVP_MD_CTX_destroy(c);
}