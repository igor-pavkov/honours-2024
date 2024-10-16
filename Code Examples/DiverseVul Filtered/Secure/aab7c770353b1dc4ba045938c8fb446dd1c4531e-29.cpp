void bn_correct_top(BIGNUM *a)
{
    BN_ULONG *ftl;
    int tmp_top = a->top;

    if (tmp_top > 0) {
        for (ftl = &(a->d[tmp_top]); tmp_top > 0; tmp_top--) {
            ftl--;
            if (*ftl != 0)
                break;
        }
        a->top = tmp_top;
    }
    if (a->top == 0)
        a->neg = 0;
    bn_pollute(a);
}