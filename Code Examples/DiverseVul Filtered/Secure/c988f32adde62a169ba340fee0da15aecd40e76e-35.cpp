void ssl_set_max_version( ssl_context *ssl, int major, int minor )
{
    if( major >= SSL_MIN_MAJOR_VERSION && major <= SSL_MAX_MAJOR_VERSION &&
        minor >= SSL_MIN_MINOR_VERSION && minor <= SSL_MAX_MINOR_VERSION )
    {
        ssl->max_major_ver = major;
        ssl->max_minor_ver = minor;
    }
}