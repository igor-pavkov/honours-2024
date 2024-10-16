static int asn1_find_end(const unsigned char **in, long len, char inf)
{
    uint32_t expected_eoc;
    long plen;
    const unsigned char *p = *in, *q;
    /* If not indefinite length constructed just add length */
    if (inf == 0) {
        *in += len;
        return 1;
    }
    expected_eoc = 1;
    /*
     * Indefinite length constructed form. Find the end when enough EOCs are
     * found. If more indefinite length constructed headers are encountered
     * increment the expected eoc count otherwise just skip to the end of the
     * data.
     */
    while (len > 0) {
        if (asn1_check_eoc(&p, len)) {
            expected_eoc--;
            if (expected_eoc == 0)
                break;
            len -= 2;
            continue;
        }
        q = p;
        /* Just read in a header: only care about the length */
        if (!asn1_check_tlen(&plen, NULL, NULL, &inf, NULL, &p, len,
                             -1, 0, 0, NULL)) {
            ERR_raise(ERR_LIB_ASN1, ERR_R_NESTED_ASN1_ERROR);
            return 0;
        }
        if (inf) {
            if (expected_eoc == UINT32_MAX) {
                ERR_raise(ERR_LIB_ASN1, ERR_R_NESTED_ASN1_ERROR);
                return 0;
            }
            expected_eoc++;
        } else {
            p += plen;
        }
        len -= p - q;
    }
    if (expected_eoc) {
        ERR_raise(ERR_LIB_ASN1, ASN1_R_MISSING_EOC);
        return 0;
    }
    *in = p;
    return 1;
}