int SSL_export_keying_material(SSL *s, unsigned char *out, size_t olen,
                               const char *label, size_t llen,
                               const unsigned char *p, size_t plen,
                               int use_context)
{
    if (s->version < TLS1_VERSION)
        return -1;

    return s->method->ssl3_enc->export_keying_material(s, out, olen, label,
                                                       llen, p, plen,
                                                       use_context);
}