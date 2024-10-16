int tls_parse_extension(SSL *s, TLSEXT_INDEX idx, int context,
                        RAW_EXTENSION *exts, X509 *x, size_t chainidx)
{
    RAW_EXTENSION *currext = &exts[idx];
    int (*parser)(SSL *s, PACKET *pkt, unsigned int context, X509 *x,
                  size_t chainidx) = NULL;

    /* Skip if the extension is not present */
    if (!currext->present)
        return 1;

    /* Skip if we've already parsed this extension */
    if (currext->parsed)
        return 1;

    currext->parsed = 1;

    if (idx < OSSL_NELEM(ext_defs)) {
        /* We are handling a built-in extension */
        const EXTENSION_DEFINITION *extdef = &ext_defs[idx];

        /* Check if extension is defined for our protocol. If not, skip */
        if (!extension_is_relevant(s, extdef->context, context))
            return 1;

        parser = s->server ? extdef->parse_ctos : extdef->parse_stoc;

        if (parser != NULL)
            return parser(s, &currext->data, context, x, chainidx);

        /*
         * If the parser is NULL we fall through to the custom extension
         * processing
         */
    }

    /* Parse custom extensions */
    return custom_ext_parse(s, context, currext->type,
                            PACKET_data(&currext->data),
                            PACKET_remaining(&currext->data),
                            x, chainidx);
}