int X509V3_get_value_bool(const CONF_VALUE *value, int *asn1_bool)
{
    const char *btmp;

    if ((btmp = value->value) == NULL)
        goto err;
    if (strcmp(btmp, "TRUE") == 0
        || strcmp(btmp, "true") == 0
        || strcmp(btmp, "Y") == 0
        || strcmp(btmp, "y") == 0
        || strcmp(btmp, "YES") == 0
        || strcmp(btmp, "yes") == 0) {
        *asn1_bool = 0xff;
        return 1;
    }
    if (strcmp(btmp, "FALSE") == 0
        || strcmp(btmp, "false") == 0
        || strcmp(btmp, "N") == 0
        || strcmp(btmp, "n") == 0
        || strcmp(btmp, "NO") == 0
        || strcmp(btmp, "no") == 0) {
        *asn1_bool = 0;
        return 1;
    }
 err:
    X509V3err(X509V3_F_X509V3_GET_VALUE_BOOL,
              X509V3_R_INVALID_BOOLEAN_STRING);
    X509V3_conf_err(value);
    return 0;
}