int BN_GF2m_mod(BIGNUM *r, const BIGNUM *a, const BIGNUM *p)
{
    int ret = 0;
    int arr[6];
    bn_check_top(a);
    bn_check_top(p);
    ret = BN_GF2m_poly2arr(p, arr, sizeof(arr) / sizeof(arr[0]));
    if (!ret || ret > (int)(sizeof(arr) / sizeof(arr[0]))) {
        BNerr(BN_F_BN_GF2M_MOD, BN_R_INVALID_LENGTH);
        return 0;
    }
    ret = BN_GF2m_mod_arr(r, a, arr);
    bn_check_top(r);
    return ret;
}