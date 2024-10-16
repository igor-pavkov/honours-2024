void ssl_set_min_version( ssl_context *ssl, int major, int minor )
{
    ssl->min_major_ver = major;
    ssl->min_minor_ver = minor;
}