static int test_x509_cmp_time_current(void)
{
    time_t now = time(NULL);
    /* Pick a day earlier and later, relative to any system clock. */
    ASN1_TIME *asn1_before = NULL, *asn1_after = NULL;
    int cmp_result, failed = 0;

    asn1_before = ASN1_TIME_adj(NULL, now, -1, 0);
    asn1_after = ASN1_TIME_adj(NULL, now, 1, 0);

    cmp_result  = X509_cmp_time(asn1_before, NULL);
    if (!TEST_int_eq(cmp_result, -1))
        failed = 1;

    cmp_result = X509_cmp_time(asn1_after, NULL);
    if (!TEST_int_eq(cmp_result, 1))
        failed = 1;

    ASN1_TIME_free(asn1_before);
    ASN1_TIME_free(asn1_after);

    return failed == 0;
}