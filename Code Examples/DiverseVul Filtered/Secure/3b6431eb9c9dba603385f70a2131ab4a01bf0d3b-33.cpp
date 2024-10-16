static const char *ltox(apr_pool_t *p, unsigned long num)
{
    if (num != 0) {
        return apr_psprintf(p, "%lx", num);
    }
    else {
        return "";
    }
}