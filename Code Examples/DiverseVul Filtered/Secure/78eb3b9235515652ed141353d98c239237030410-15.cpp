static req_fun_t *makefun(const void *fun, int type, apr_pool_t *pool)
{
    req_fun_t *rft = apr_palloc(pool, sizeof(req_fun_t));
    rft->fun = fun;
    rft->type = type;
    return rft;
}