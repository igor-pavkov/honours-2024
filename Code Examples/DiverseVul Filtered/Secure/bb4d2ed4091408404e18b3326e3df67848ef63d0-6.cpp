int X509V3_add_value(const char *name, const char *value,
                     STACK_OF(CONF_VALUE) **extlist)
{
    return x509v3_add_len_value(name, value,
                                value != NULL ? strlen((const char *)value) : 0,
                                extlist);
}