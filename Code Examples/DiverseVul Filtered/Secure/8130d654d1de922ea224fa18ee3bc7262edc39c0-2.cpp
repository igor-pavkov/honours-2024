int X509_add_cert(STACK_OF(X509) *sk, X509 *cert, int flags)
{
    if (sk == NULL) {
        ERR_raise(ERR_LIB_X509, ERR_R_PASSED_NULL_PARAMETER);
        return 0;
    }
    if ((flags & X509_ADD_FLAG_NO_DUP) != 0) {
        /*
         * not using sk_X509_set_cmp_func() and sk_X509_find()
         * because this re-orders the certs on the stack
         */
        int i;

        for (i = 0; i < sk_X509_num(sk); i++) {
            if (X509_cmp(sk_X509_value(sk, i), cert) == 0)
                return 1;
        }
    }
    if ((flags & X509_ADD_FLAG_NO_SS) != 0 && X509_self_signed(cert, 0))
        return 1;
    if (!sk_X509_insert(sk, cert,
                        (flags & X509_ADD_FLAG_PREPEND) != 0 ? 0 : -1)) {
        ERR_raise(ERR_LIB_X509, ERR_R_MALLOC_FAILURE);
        return 0;
    }
    if ((flags & X509_ADD_FLAG_UP_REF) != 0)
        (void)X509_up_ref(cert);
    return 1;
}