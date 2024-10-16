void md5_final(unsigned char *md, MD5CTX c)
{
    unsigned int mdlen = 0;

    EVP_DigestFinal(c, md, &mdlen);
    EVP_MD_CTX_destroy(c);
}