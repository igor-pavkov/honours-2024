static int check_suite_b(EVP_PKEY *pkey, int sign_nid, unsigned long *pflags)
{
    char curve_name[80];
    size_t curve_name_len;
    int curve_nid;

    if (pkey == NULL || !EVP_PKEY_is_a(pkey, "EC"))
        return X509_V_ERR_SUITE_B_INVALID_ALGORITHM;

    if (!EVP_PKEY_get_group_name(pkey, curve_name, sizeof(curve_name),
                                 &curve_name_len))
        return X509_V_ERR_SUITE_B_INVALID_CURVE;

    curve_nid = OBJ_txt2nid(curve_name);
    /* Check curve is consistent with LOS */
    if (curve_nid == NID_secp384r1) { /* P-384 */
        /*
         * Check signature algorithm is consistent with curve.
         */
        if (sign_nid != -1 && sign_nid != NID_ecdsa_with_SHA384)
            return X509_V_ERR_SUITE_B_INVALID_SIGNATURE_ALGORITHM;
        if (!(*pflags & X509_V_FLAG_SUITEB_192_LOS))
            return X509_V_ERR_SUITE_B_LOS_NOT_ALLOWED;
        /* If we encounter P-384 we cannot use P-256 later */
        *pflags &= ~X509_V_FLAG_SUITEB_128_LOS_ONLY;
    } else if (curve_nid == NID_X9_62_prime256v1) { /* P-256 */
        if (sign_nid != -1 && sign_nid != NID_ecdsa_with_SHA256)
            return X509_V_ERR_SUITE_B_INVALID_SIGNATURE_ALGORITHM;
        if (!(*pflags & X509_V_FLAG_SUITEB_128_LOS_ONLY))
            return X509_V_ERR_SUITE_B_LOS_NOT_ALLOWED;
    } else {
        return X509_V_ERR_SUITE_B_INVALID_CURVE;
    }
    return X509_V_OK;
}