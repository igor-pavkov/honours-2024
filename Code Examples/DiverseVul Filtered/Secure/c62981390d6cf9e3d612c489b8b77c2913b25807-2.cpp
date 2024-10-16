void *ASN1_item_d2i_fp(const ASN1_ITEM *it, FILE *in, void *x)
{
    BIO *b;
    char *ret;

    if ((b = BIO_new(BIO_s_file())) == NULL) {
        ASN1err(ASN1_F_ASN1_ITEM_D2I_FP, ERR_R_BUF_LIB);
        return (NULL);
    }
    BIO_set_fp(b, in, BIO_NOCLOSE);
    ret = ASN1_item_d2i_bio(it, b, x);
    BIO_free(b);
    return (ret);
}