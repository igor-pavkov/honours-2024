int BN_GF2m_mod_inv(BIGNUM *r, const BIGNUM *a, const BIGNUM *p, BN_CTX *ctx)
{
    BIGNUM *b, *c = NULL, *u = NULL, *v = NULL, *tmp;
    int ret = 0;

    bn_check_top(a);
    bn_check_top(p);

    BN_CTX_start(ctx);

    if ((b = BN_CTX_get(ctx)) == NULL)
        goto err;
    if ((c = BN_CTX_get(ctx)) == NULL)
        goto err;
    if ((u = BN_CTX_get(ctx)) == NULL)
        goto err;
    if ((v = BN_CTX_get(ctx)) == NULL)
        goto err;

    if (!BN_GF2m_mod(u, a, p))
        goto err;
    if (BN_is_zero(u))
        goto err;

    if (!BN_copy(v, p))
        goto err;
# if 0
    if (!BN_one(b))
        goto err;

    while (1) {
        while (!BN_is_odd(u)) {
            if (BN_is_zero(u))
                goto err;
            if (!BN_rshift1(u, u))
                goto err;
            if (BN_is_odd(b)) {
                if (!BN_GF2m_add(b, b, p))
                    goto err;
            }
            if (!BN_rshift1(b, b))
                goto err;
        }

        if (BN_abs_is_word(u, 1))
            break;

        if (BN_num_bits(u) < BN_num_bits(v)) {
            tmp = u;
            u = v;
            v = tmp;
            tmp = b;
            b = c;
            c = tmp;
        }

        if (!BN_GF2m_add(u, u, v))
            goto err;
        if (!BN_GF2m_add(b, b, c))
            goto err;
    }
# else
    {
        int i, ubits = BN_num_bits(u), vbits = BN_num_bits(v), /* v is copy
                                                                * of p */
            top = p->top;
        BN_ULONG *udp, *bdp, *vdp, *cdp;

        bn_wexpand(u, top);
        udp = u->d;
        for (i = u->top; i < top; i++)
            udp[i] = 0;
        u->top = top;
        bn_wexpand(b, top);
        bdp = b->d;
        bdp[0] = 1;
        for (i = 1; i < top; i++)
            bdp[i] = 0;
        b->top = top;
        bn_wexpand(c, top);
        cdp = c->d;
        for (i = 0; i < top; i++)
            cdp[i] = 0;
        c->top = top;
        vdp = v->d;             /* It pays off to "cache" *->d pointers,
                                 * because it allows optimizer to be more
                                 * aggressive. But we don't have to "cache"
                                 * p->d, because *p is declared 'const'... */
        while (1) {
            while (ubits && !(udp[0] & 1)) {
                BN_ULONG u0, u1, b0, b1, mask;

                u0 = udp[0];
                b0 = bdp[0];
                mask = (BN_ULONG)0 - (b0 & 1);
                b0 ^= p->d[0] & mask;
                for (i = 0; i < top - 1; i++) {
                    u1 = udp[i + 1];
                    udp[i] = ((u0 >> 1) | (u1 << (BN_BITS2 - 1))) & BN_MASK2;
                    u0 = u1;
                    b1 = bdp[i + 1] ^ (p->d[i + 1] & mask);
                    bdp[i] = ((b0 >> 1) | (b1 << (BN_BITS2 - 1))) & BN_MASK2;
                    b0 = b1;
                }
                udp[i] = u0 >> 1;
                bdp[i] = b0 >> 1;
                ubits--;
            }

            if (ubits <= BN_BITS2 && udp[0] == 1)
                break;

            if (ubits < vbits) {
                i = ubits;
                ubits = vbits;
                vbits = i;
                tmp = u;
                u = v;
                v = tmp;
                tmp = b;
                b = c;
                c = tmp;
                udp = vdp;
                vdp = v->d;
                bdp = cdp;
                cdp = c->d;
            }
            for (i = 0; i < top; i++) {
                udp[i] ^= vdp[i];
                bdp[i] ^= cdp[i];
            }
            if (ubits == vbits) {
                BN_ULONG ul;
                int utop = (ubits - 1) / BN_BITS2;

                while ((ul = udp[utop]) == 0 && utop)
                    utop--;
                ubits = utop * BN_BITS2 + BN_num_bits_word(ul);
            }
        }
        bn_correct_top(b);
    }
# endif

    if (!BN_copy(r, b))
        goto err;
    bn_check_top(r);
    ret = 1;

 err:
# ifdef BN_DEBUG                /* BN_CTX_end would complain about the
                                 * expanded form */
    bn_correct_top(c);
    bn_correct_top(u);
    bn_correct_top(v);
# endif
    BN_CTX_end(ctx);
    return ret;
}