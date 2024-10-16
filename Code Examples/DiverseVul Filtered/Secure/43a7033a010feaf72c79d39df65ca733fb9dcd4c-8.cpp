static int asn1_collect(BUF_MEM *buf, const unsigned char **in, long len,
                        char inf, int tag, int aclass, int depth)
{
    const unsigned char *p, *q;
    long plen;
    char cst, ininf;
    p = *in;
    inf &= 1;
    /*
     * If no buffer and not indefinite length constructed just pass over the
     * encoded data
     */
    if (!buf && !inf) {
        *in += len;
        return 1;
    }
    while (len > 0) {
        q = p;
        /* Check for EOC */
        if (asn1_check_eoc(&p, len)) {
            /*
             * EOC is illegal outside indefinite length constructed form
             */
            if (!inf) {
                ERR_raise(ERR_LIB_ASN1, ASN1_R_UNEXPECTED_EOC);
                return 0;
            }
            inf = 0;
            break;
        }

        if (!asn1_check_tlen(&plen, NULL, NULL, &ininf, &cst, &p,
                             len, tag, aclass, 0, NULL)) {
            ERR_raise(ERR_LIB_ASN1, ERR_R_NESTED_ASN1_ERROR);
            return 0;
        }

        /* If indefinite length constructed update max length */
        if (cst) {
            if (depth >= ASN1_MAX_STRING_NEST) {
                ERR_raise(ERR_LIB_ASN1, ASN1_R_NESTED_ASN1_STRING);
                return 0;
            }
            if (!asn1_collect(buf, &p, plen, ininf, tag, aclass, depth + 1))
                return 0;
        } else if (plen && !collect_data(buf, &p, plen))
            return 0;
        len -= p - q;
    }
    if (inf) {
        ERR_raise(ERR_LIB_ASN1, ASN1_R_MISSING_EOC);
        return 0;
    }
    *in = p;
    return 1;
}