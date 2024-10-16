static int test_x509_time_print(int idx)
{
    BIO *m;
    int ret = 0, rv;
    char *pp;
    const char *readable;

    if (!TEST_ptr(m = BIO_new(BIO_s_mem())))
        goto err;

    rv = ASN1_TIME_print(m, &x509_print_tests[idx].asn1);
    readable = x509_print_tests[idx].readable;

    if (rv == 0 && !TEST_str_eq(readable, "Bad time value")) {
        /* only if the test case intends to fail... */
        goto err;
    }
    if (!TEST_int_ne(rv = BIO_get_mem_data(m, &pp), 0)
        || !TEST_int_eq(rv, (int)strlen(readable))
        || !TEST_strn_eq(pp, readable, rv))
        goto err;

    ret = 1;
 err:
    BIO_free(m);
    return ret;
}