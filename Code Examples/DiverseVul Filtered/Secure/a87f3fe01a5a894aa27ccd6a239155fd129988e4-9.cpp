static int tls1_check_pkey_comp(SSL *s, EVP_PKEY *pkey)
{
    const EC_KEY *ec;
    const EC_GROUP *grp;
    unsigned char comp_id;
    size_t i;

    /* If not an EC key nothing to check */
    if (!EVP_PKEY_is_a(pkey, "EC"))
        return 1;
    ec = EVP_PKEY_get0_EC_KEY(pkey);
    grp = EC_KEY_get0_group(ec);

    /* Get required compression id */
    if (EC_KEY_get_conv_form(ec) == POINT_CONVERSION_UNCOMPRESSED) {
            comp_id = TLSEXT_ECPOINTFORMAT_uncompressed;
    } else if (SSL_IS_TLS13(s)) {
            /*
             * ec_point_formats extension is not used in TLSv1.3 so we ignore
             * this check.
             */
            return 1;
    } else {
        int field_type = EC_METHOD_get_field_type(EC_GROUP_method_of(grp));

        if (field_type == NID_X9_62_prime_field)
            comp_id = TLSEXT_ECPOINTFORMAT_ansiX962_compressed_prime;
        else if (field_type == NID_X9_62_characteristic_two_field)
            comp_id = TLSEXT_ECPOINTFORMAT_ansiX962_compressed_char2;
        else
            return 0;
    }
    /*
     * If point formats extension present check it, otherwise everything is
     * supported (see RFC4492).
     */
    if (s->ext.peer_ecpointformats == NULL)
        return 1;

    for (i = 0; i < s->ext.peer_ecpointformats_len; i++) {
        if (s->ext.peer_ecpointformats[i] == comp_id)
            return 1;
    }
    return 0;
}