static void doall_util_fn(OPENSSL_LHASH *lh, int use_arg,
                          OPENSSL_LH_DOALL_FUNC func,
                          OPENSSL_LH_DOALL_FUNCARG func_arg, void *arg)
{
    int i;
    OPENSSL_LH_NODE *a, *n;

    if (lh == NULL)
        return;

    /*
     * reverse the order so we search from 'top to bottom' We were having
     * memory leaks otherwise
     */
    for (i = lh->num_nodes - 1; i >= 0; i--) {
        a = lh->b[i];
        while (a != NULL) {
            n = a->next;
            if (use_arg)
                func_arg(a->data, arg);
            else
                func(a->data);
            a = n;
        }
    }
}