int X509V3_get_value_int(const CONF_VALUE *value, ASN1_INTEGER **aint)
{
    ASN1_INTEGER *itmp;

    if ((itmp = s2i_ASN1_INTEGER(NULL, value->value)) == NULL) {
        X509V3_conf_err(value);
        return 0;
    }
    *aint = itmp;
    return 1;
}