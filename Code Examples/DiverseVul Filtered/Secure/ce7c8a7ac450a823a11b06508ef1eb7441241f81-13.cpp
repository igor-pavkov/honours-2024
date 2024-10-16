void test_urldecode(const char *v1, const char *v2)
{
    char *v = strdup(v1);
    _q_urldecode(v);
    ASSERT_EQUAL_STR(v, v2);
    free(v);
}