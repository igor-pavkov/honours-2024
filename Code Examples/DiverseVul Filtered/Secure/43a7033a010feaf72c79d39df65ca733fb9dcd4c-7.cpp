static int asn1_template_ex_d2i(ASN1_VALUE **val,
                                const unsigned char **in, long inlen,
                                const ASN1_TEMPLATE *tt, char opt,
                                ASN1_TLC *ctx, int depth)
{
    int flags, aclass;
    int ret;
    long len;
    const unsigned char *p, *q;
    char exp_eoc;
    if (!val)
        return 0;
    flags = tt->flags;
    aclass = flags & ASN1_TFLG_TAG_CLASS;

    p = *in;

    /* Check if EXPLICIT tag expected */
    if (flags & ASN1_TFLG_EXPTAG) {
        char cst;
        /*
         * Need to work out amount of data available to the inner content and
         * where it starts: so read in EXPLICIT header to get the info.
         */
        ret = asn1_check_tlen(&len, NULL, NULL, &exp_eoc, &cst,
                              &p, inlen, tt->tag, aclass, opt, ctx);
        q = p;
        if (!ret) {
            ERR_raise(ERR_LIB_ASN1, ERR_R_NESTED_ASN1_ERROR);
            return 0;
        } else if (ret == -1)
            return -1;
        if (!cst) {
            ERR_raise(ERR_LIB_ASN1, ASN1_R_EXPLICIT_TAG_NOT_CONSTRUCTED);
            return 0;
        }
        /* We've found the field so it can't be OPTIONAL now */
        ret = asn1_template_noexp_d2i(val, &p, len, tt, 0, ctx, depth);
        if (!ret) {
            ERR_raise(ERR_LIB_ASN1, ERR_R_NESTED_ASN1_ERROR);
            return 0;
        }
        /* We read the field in OK so update length */
        len -= p - q;
        if (exp_eoc) {
            /* If NDEF we must have an EOC here */
            if (!asn1_check_eoc(&p, len)) {
                ERR_raise(ERR_LIB_ASN1, ASN1_R_MISSING_EOC);
                goto err;
            }
        } else {
            /*
             * Otherwise we must hit the EXPLICIT tag end or its an error
             */
            if (len) {
                ERR_raise(ERR_LIB_ASN1, ASN1_R_EXPLICIT_LENGTH_MISMATCH);
                goto err;
            }
        }
    } else
        return asn1_template_noexp_d2i(val, in, inlen, tt, opt, ctx, depth);

    *in = p;
    return 1;

 err:
    return 0;
}