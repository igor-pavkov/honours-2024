int setup_tests(void)
{
    ADD_TEST(test_x509_cmp_time_current);
    ADD_ALL_TESTS(test_x509_cmp_time, OSSL_NELEM(x509_cmp_tests));
    ADD_ALL_TESTS(test_x509_time, OSSL_NELEM(x509_format_tests));
    ADD_ALL_TESTS(test_days, OSSL_NELEM(day_of_week_tests));
    ADD_ALL_TESTS(test_x509_time_print, OSSL_NELEM(x509_print_tests));
    return 1;
}