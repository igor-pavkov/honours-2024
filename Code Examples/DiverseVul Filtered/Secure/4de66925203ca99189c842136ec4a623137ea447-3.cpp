static int test_x509_cmp_time(int idx)
{
    ASN1_TIME t;
    int result;

    memset(&t, 0, sizeof(t));
    t.type = x509_cmp_tests[idx].type;
    t.data = (unsigned char*)(x509_cmp_tests[idx].data);
    t.length = strlen(x509_cmp_tests[idx].data);
    t.flags = 0;

    result = X509_cmp_time(&t, &x509_cmp_tests[idx].cmp_time);
    if (!TEST_int_eq(result, x509_cmp_tests[idx].expected)) {
        TEST_info("test_x509_cmp_time(%d) failed: expected %d, got %d\n",
                idx, x509_cmp_tests[idx].expected, result);
        return 0;
    }
    return 1;
}