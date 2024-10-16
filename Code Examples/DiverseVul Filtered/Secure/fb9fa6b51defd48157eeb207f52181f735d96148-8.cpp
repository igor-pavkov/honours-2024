int tls_validate_all_contexts(SSL *s, unsigned int thisctx, RAW_EXTENSION *exts)
{
    size_t i, num_exts, builtin_num = OSSL_NELEM(ext_defs), offset;
    RAW_EXTENSION *thisext;
    unsigned int context;
    ENDPOINT role = ENDPOINT_BOTH;

    if ((thisctx & SSL_EXT_CLIENT_HELLO) != 0)
        role = ENDPOINT_SERVER;
    else if ((thisctx & SSL_EXT_TLS1_2_SERVER_HELLO) != 0)
        role = ENDPOINT_CLIENT;

    /* Calculate the number of extensions in the extensions list */
    num_exts = builtin_num + s->cert->custext.meths_count;

    for (thisext = exts, i = 0; i < num_exts; i++, thisext++) {
        if (!thisext->present)
            continue;

        if (i < builtin_num) {
            context = ext_defs[i].context;
        } else {
            custom_ext_method *meth = NULL;

            meth = custom_ext_find(&s->cert->custext, role, thisext->type,
                                   &offset);
            if (!ossl_assert(meth != NULL))
                return 0;
            context = meth->context;
        }

        if (!validate_context(s, context, thisctx))
            return 0;
    }

    return 1;
}