void X509_email_free(STACK_OF(OPENSSL_STRING) *sk)
{
    sk_OPENSSL_STRING_pop_free(sk, str_free);
}