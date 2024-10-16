static int asn1_set_seq_out(STACK_OF(const_ASN1_VALUE) *sk,
                            unsigned char **out,
                            int skcontlen, const ASN1_ITEM *item,
                            int do_sort, int iclass)
{
    int i;
    const ASN1_VALUE *skitem;
    unsigned char *tmpdat = NULL, *p = NULL;
    DER_ENC *derlst = NULL, *tder;
    if (do_sort) {
        /* Don't need to sort less than 2 items */
        if (sk_const_ASN1_VALUE_num(sk) < 2)
            do_sort = 0;
        else {
            derlst = OPENSSL_malloc(sk_const_ASN1_VALUE_num(sk)
                                    * sizeof(*derlst));
            if (derlst == NULL)
                return 0;
            tmpdat = OPENSSL_malloc(skcontlen);
            if (tmpdat == NULL) {
                OPENSSL_free(derlst);
                return 0;
            }
        }
    }
    /* If not sorting just output each item */
    if (!do_sort) {
        for (i = 0; i < sk_const_ASN1_VALUE_num(sk); i++) {
            skitem = sk_const_ASN1_VALUE_value(sk, i);
            ASN1_item_ex_i2d(&skitem, out, item, -1, iclass);
        }
        return 1;
    }
    p = tmpdat;

    /* Doing sort: build up a list of each member's DER encoding */
    for (i = 0, tder = derlst; i < sk_const_ASN1_VALUE_num(sk); i++, tder++) {
        skitem = sk_const_ASN1_VALUE_value(sk, i);
        tder->data = p;
        tder->length = ASN1_item_ex_i2d(&skitem, &p, item, -1, iclass);
        tder->field = skitem;
    }

    /* Now sort them */
    qsort(derlst, sk_const_ASN1_VALUE_num(sk), sizeof(*derlst), der_cmp);
    /* Output sorted DER encoding */
    p = *out;
    for (i = 0, tder = derlst; i < sk_const_ASN1_VALUE_num(sk); i++, tder++) {
        memcpy(p, tder->data, tder->length);
        p += tder->length;
    }
    *out = p;
    /* If do_sort is 2 then reorder the STACK */
    if (do_sort == 2) {
        for (i = 0, tder = derlst; i < sk_const_ASN1_VALUE_num(sk); i++, tder++)
            (void)sk_const_ASN1_VALUE_set(sk, i, tder->field);
    }
    OPENSSL_free(derlst);
    OPENSSL_free(tmpdat);
    return 1;
}