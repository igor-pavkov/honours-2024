int X509V3_NAME_from_section(X509_NAME *nm, STACK_OF(CONF_VALUE) *dn_sk,
                             unsigned long chtype)
{
    CONF_VALUE *v;
    int i, mval, spec_char, plus_char;
    char *p, *type;
    if (!nm)
        return 0;

    for (i = 0; i < sk_CONF_VALUE_num(dn_sk); i++) {
        v = sk_CONF_VALUE_value(dn_sk, i);
        type = v->name;
        /*
         * Skip past any leading X. X: X, etc to allow for multiple instances
         */
        for (p = type; *p; p++) {
#ifndef CHARSET_EBCDIC
            spec_char = ((*p == ':') || (*p == ',') || (*p == '.'));
#else
            spec_char = ((*p == os_toascii[':']) || (*p == os_toascii[','])
                    || (*p == os_toascii['.']));
#endif
            if (spec_char) {
                p++;
                if (*p)
                    type = p;
                break;
            }
        }
#ifndef CHARSET_EBCDIC
        plus_char = (*type == '+');
#else
        plus_char = (*type == os_toascii['+']);
#endif
        if (plus_char) {
            mval = -1;
            type++;
        } else
            mval = 0;
        if (!X509_NAME_add_entry_by_txt(nm, type, chtype,
                                        (unsigned char *)v->value, -1, -1,
                                        mval))
            return 0;

    }
    return 1;
}