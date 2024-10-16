int x509v3_add_len_value_uchar(const char *name, const unsigned char *value,
                               size_t vallen, STACK_OF(CONF_VALUE) **extlist)
{
    return x509v3_add_len_value(name, (const char *)value, vallen, extlist);
}