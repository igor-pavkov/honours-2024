void ssl_set_max_version( ssl_context *ssl, int major, int minor )
{
    ssl->max_major_ver = major;
    ssl->max_minor_ver = minor;
}