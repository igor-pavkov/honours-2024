static int init_psk_kex_modes(SSL *s, unsigned int context)
{
    s->ext.psk_kex_mode = TLSEXT_KEX_MODE_FLAG_NONE;
    return 1;
}