int X509_CRL_check_suiteb(X509_CRL *crl, EVP_PKEY *pk, unsigned long flags)
{
    int sign_nid;
    if (!(flags & X509_V_FLAG_SUITEB_128_LOS))
        return X509_V_OK;
    sign_nid = OBJ_obj2nid(crl->crl.sig_alg.algorithm);
    return check_suite_b(pk, sign_nid, &flags);
}