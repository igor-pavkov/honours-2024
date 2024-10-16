int X509V3_add_value_bool(const char *name, int asn1_bool,
                          STACK_OF(CONF_VALUE) **extlist)
{
    if (asn1_bool)
        return X509V3_add_value(name, "TRUE", extlist);
    return X509V3_add_value(name, "FALSE", extlist);
}