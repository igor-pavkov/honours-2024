int tls_parse_all_extensions(SSL *s, int context, RAW_EXTENSION *exts, X509 *x,
                             size_t chainidx, int fin)
{
    size_t i, numexts = OSSL_NELEM(ext_defs);
    const EXTENSION_DEFINITION *thisexd;

    /* Calculate the number of extensions in the extensions list */
    numexts += s->cert->custext.meths_count;

    /* Parse each extension in turn */
    for (i = 0; i < numexts; i++) {
        if (!tls_parse_extension(s, i, context, exts, x, chainidx)) {
            /* SSLfatal() already called */
            return 0;
        }
    }

    if (fin) {
        /*
         * Finalise all known extensions relevant to this context,
         * whether we have found them or not
         */
        for (i = 0, thisexd = ext_defs; i < OSSL_NELEM(ext_defs);
             i++, thisexd++) {
            if (thisexd->final != NULL && (thisexd->context & context) != 0
                && !thisexd->final(s, context, exts[i].present)) {
                /* SSLfatal() already called */
                return 0;
            }
        }
    }

    return 1;
}