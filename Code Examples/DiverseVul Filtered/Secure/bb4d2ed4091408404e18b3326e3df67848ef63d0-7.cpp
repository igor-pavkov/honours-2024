ASN1_INTEGER *s2i_ASN1_INTEGER(X509V3_EXT_METHOD *method, const char *value)
{
    BIGNUM *bn = NULL;
    ASN1_INTEGER *aint;
    int isneg, ishex;
    int ret;
    if (value == NULL) {
        X509V3err(X509V3_F_S2I_ASN1_INTEGER, X509V3_R_INVALID_NULL_VALUE);
        return NULL;
    }
    bn = BN_new();
    if (bn == NULL) {
        X509V3err(X509V3_F_S2I_ASN1_INTEGER, ERR_R_MALLOC_FAILURE);
        return NULL;
    }
    if (value[0] == '-') {
        value++;
        isneg = 1;
    } else
        isneg = 0;

    if (value[0] == '0' && ((value[1] == 'x') || (value[1] == 'X'))) {
        value += 2;
        ishex = 1;
    } else
        ishex = 0;

    if (ishex)
        ret = BN_hex2bn(&bn, value);
    else
        ret = BN_dec2bn(&bn, value);

    if (!ret || value[ret]) {
        BN_free(bn);
        X509V3err(X509V3_F_S2I_ASN1_INTEGER, X509V3_R_BN_DEC2BN_ERROR);
        return NULL;
    }

    if (isneg && BN_is_zero(bn))
        isneg = 0;

    aint = BN_to_ASN1_INTEGER(bn, NULL);
    BN_free(bn);
    if (!aint) {
        X509V3err(X509V3_F_S2I_ASN1_INTEGER,
                  X509V3_R_BN_TO_ASN1_INTEGER_ERROR);
        return NULL;
    }
    if (isneg)
        aint->type |= V_ASN1_NEG;
    return aint;
}