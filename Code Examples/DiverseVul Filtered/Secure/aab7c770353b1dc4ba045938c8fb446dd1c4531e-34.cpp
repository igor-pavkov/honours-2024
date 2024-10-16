void BN_swap(BIGNUM *a, BIGNUM *b)
{
    int flags_old_a, flags_old_b;
    BN_ULONG *tmp_d;
    int tmp_top, tmp_dmax, tmp_neg;

    bn_check_top(a);
    bn_check_top(b);

    flags_old_a = a->flags;
    flags_old_b = b->flags;

    tmp_d = a->d;
    tmp_top = a->top;
    tmp_dmax = a->dmax;
    tmp_neg = a->neg;

    a->d = b->d;
    a->top = b->top;
    a->dmax = b->dmax;
    a->neg = b->neg;

    b->d = tmp_d;
    b->top = tmp_top;
    b->dmax = tmp_dmax;
    b->neg = tmp_neg;

    a->flags =
        (flags_old_a & BN_FLG_MALLOCED) | (flags_old_b & BN_FLG_STATIC_DATA);
    b->flags =
        (flags_old_b & BN_FLG_MALLOCED) | (flags_old_a & BN_FLG_STATIC_DATA);
    bn_check_top(a);
    bn_check_top(b);
}