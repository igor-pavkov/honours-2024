int ssl_undefined_const_function(const SSL *s)
{
    SSLerr(SSL_F_SSL_UNDEFINED_CONST_FUNCTION,
           ERR_R_SHOULD_NOT_HAVE_BEEN_CALLED);
    return (0);
}