static uint16_t tls1_get_group_id(EVP_PKEY *pkey)
{
    int curve_nid = evp_pkey_get_EC_KEY_curve_nid(pkey);

    if (curve_nid == NID_undef)
        return 0;
    return tls1_nid2group_id(curve_nid);
}