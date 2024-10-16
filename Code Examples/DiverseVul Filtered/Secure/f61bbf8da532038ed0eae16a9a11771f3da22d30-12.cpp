int BN_GF2m_mod_div(BIGNUM *r, const BIGNUM *y, const BIGNUM *x,
                    const BIGNUM *p, BN_CTX *ctx)
{
    BIGNUM *a, *b, *u, *v;
    int ret = 0;

    bn_check_top(y);
    bn_check_top(x);
    bn_check_top(p);

    BN_CTX_start(ctx);

    a = BN_CTX_get(ctx);
    b = BN_CTX_get(ctx);
    u = BN_CTX_get(ctx);
    v = BN_CTX_get(ctx);
    if (v == NULL)
        goto err;

    /* reduce x and y mod p */
    if (!BN_GF2m_mod(u, y, p))
        goto err;
    if (!BN_GF2m_mod(a, x, p))
        goto err;
    if (!BN_copy(b, p))
        goto err;

    while (!BN_is_odd(a)) {
        if (!BN_rshift1(a, a))
            goto err;
        if (BN_is_odd(u))
            if (!BN_GF2m_add(u, u, p))
                goto err;
        if (!BN_rshift1(u, u))
            goto err;
    }

    do {
        if (BN_GF2m_cmp(b, a) > 0) {
            if (!BN_GF2m_add(b, b, a))
                goto err;
            if (!BN_GF2m_add(v, v, u))
                goto err;
            do {
                if (!BN_rshift1(b, b))
                    goto err;
                if (BN_is_odd(v))
                    if (!BN_GF2m_add(v, v, p))
                        goto err;
                if (!BN_rshift1(v, v))
                    goto err;
            } while (!BN_is_odd(b));
        } else if (BN_abs_is_word(a, 1))
            break;
        else {
            if (!BN_GF2m_add(a, a, b))
                goto err;
            if (!BN_GF2m_add(u, u, v))
                goto err;
            do {
                if (!BN_rshift1(a, a))
                    goto err;
                if (BN_is_odd(u))
                    if (!BN_GF2m_add(u, u, p))
                        goto err;
                if (!BN_rshift1(u, u))
                    goto err;
            } while (!BN_is_odd(a));
        }
    } while (1);

    if (!BN_copy(r, u))
        goto err;
    bn_check_top(r);
    ret = 1;

 err:
    BN_CTX_end(ctx);
    return ret;
}