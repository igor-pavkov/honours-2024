ASN1_VALUE *ASN1_item_d2i(ASN1_VALUE **pval,
                          const unsigned char **in, long len,
                          const ASN1_ITEM *it)
{
    ASN1_TLC c;
    ASN1_VALUE *ptmpval = NULL;

    if (pval == NULL)
        pval = &ptmpval;
    asn1_tlc_clear_nc(&c);
    if (ASN1_item_ex_d2i(pval, in, len, it, -1, 0, 0, &c) > 0)
        return *pval;
    return NULL;
}