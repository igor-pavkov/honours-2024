int X509V3_add_value_uchar(const char *name, const unsigned char *value,
                           STACK_OF(CONF_VALUE) **extlist)
{
    return x509v3_add_len_value(name, (const char *)value,
                                value != NULL ? strlen((const char *)value) : 0,
                                extlist);
}