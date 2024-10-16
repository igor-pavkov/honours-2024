static int verify_extension(SSL *s, unsigned int context, unsigned int type,
                            custom_ext_methods *meths, RAW_EXTENSION *rawexlist,
                            RAW_EXTENSION **found)
{
    size_t i;
    size_t builtin_num = OSSL_NELEM(ext_defs);
    const EXTENSION_DEFINITION *thisext;

    for (i = 0, thisext = ext_defs; i < builtin_num; i++, thisext++) {
        if (type == thisext->type) {
            if (!validate_context(s, thisext->context, context))
                return 0;

            *found = &rawexlist[i];
            return 1;
        }
    }

    /* Check the custom extensions */
    if (meths != NULL) {
        size_t offset = 0;
        ENDPOINT role = ENDPOINT_BOTH;
        custom_ext_method *meth = NULL;

        if ((context & SSL_EXT_CLIENT_HELLO) != 0)
            role = ENDPOINT_SERVER;
        else if ((context & SSL_EXT_TLS1_2_SERVER_HELLO) != 0)
            role = ENDPOINT_CLIENT;

        meth = custom_ext_find(meths, role, type, &offset);
        if (meth != NULL) {
            if (!validate_context(s, meth->context, context))
                return 0;
            *found = &rawexlist[offset + builtin_num];
            return 1;
        }
    }

    /* Unknown extension. We allow it */
    *found = NULL;
    return 1;
}