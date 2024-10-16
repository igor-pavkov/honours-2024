static int test_days(int n)
{
    char d[16];
    ASN1_TIME *a = NULL;
    struct tm t;
    int r;

    BIO_snprintf(d, sizeof(d), "%04d%02d%02d050505Z",
                 day_of_week_tests[n].y, day_of_week_tests[n].m,
                 day_of_week_tests[n].d);

    if (!TEST_ptr(a = ASN1_TIME_new()))
        return 0;

    r = TEST_true(ASN1_TIME_set_string(a, d))
        && TEST_true(ASN1_TIME_to_tm(a, &t))
        && TEST_int_eq(t.tm_yday, day_of_week_tests[n].yd)
        && TEST_int_eq(t.tm_wday, day_of_week_tests[n].wd);

    ASN1_TIME_free(a);
    return r;
}