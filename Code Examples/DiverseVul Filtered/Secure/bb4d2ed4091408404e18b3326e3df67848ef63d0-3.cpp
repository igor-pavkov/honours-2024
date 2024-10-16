STACK_OF(OPENSSL_STRING) *X509_get1_ocsp(X509 *x)
{
    AUTHORITY_INFO_ACCESS *info;
    STACK_OF(OPENSSL_STRING) *ret = NULL;
    int i;

    info = X509_get_ext_d2i(x, NID_info_access, NULL, NULL);
    if (!info)
        return NULL;
    for (i = 0; i < sk_ACCESS_DESCRIPTION_num(info); i++) {
        ACCESS_DESCRIPTION *ad = sk_ACCESS_DESCRIPTION_value(info, i);
        if (OBJ_obj2nid(ad->method) == NID_ad_OCSP) {
            if (ad->location->type == GEN_URI) {
                if (!append_ia5
                    (&ret, ad->location->d.uniformResourceIdentifier))
                    break;
            }
        }
    }
    AUTHORITY_INFO_ACCESS_free(info);
    return ret;
}