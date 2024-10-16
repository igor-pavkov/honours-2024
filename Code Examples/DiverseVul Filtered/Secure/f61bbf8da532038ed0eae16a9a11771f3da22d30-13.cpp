static void bn_GF2m_mul_1x1(BN_ULONG *r1, BN_ULONG *r0, const BN_ULONG a,
                            const BN_ULONG b)
{
    register BN_ULONG h, l, s;
    BN_ULONG tab[8], top2b = a >> 30;
    register BN_ULONG a1, a2, a4;

    a1 = a & (0x3FFFFFFF);
    a2 = a1 << 1;
    a4 = a2 << 1;

    tab[0] = 0;
    tab[1] = a1;
    tab[2] = a2;
    tab[3] = a1 ^ a2;
    tab[4] = a4;
    tab[5] = a1 ^ a4;
    tab[6] = a2 ^ a4;
    tab[7] = a1 ^ a2 ^ a4;

    s = tab[b & 0x7];
    l = s;
    s = tab[b >> 3 & 0x7];
    l ^= s << 3;
    h = s >> 29;
    s = tab[b >> 6 & 0x7];
    l ^= s << 6;
    h ^= s >> 26;
    s = tab[b >> 9 & 0x7];
    l ^= s << 9;
    h ^= s >> 23;
    s = tab[b >> 12 & 0x7];
    l ^= s << 12;
    h ^= s >> 20;
    s = tab[b >> 15 & 0x7];
    l ^= s << 15;
    h ^= s >> 17;
    s = tab[b >> 18 & 0x7];
    l ^= s << 18;
    h ^= s >> 14;
    s = tab[b >> 21 & 0x7];
    l ^= s << 21;
    h ^= s >> 11;
    s = tab[b >> 24 & 0x7];
    l ^= s << 24;
    h ^= s >> 8;
    s = tab[b >> 27 & 0x7];
    l ^= s << 27;
    h ^= s >> 5;
    s = tab[b >> 30];
    l ^= s << 30;
    h ^= s >> 2;

    /* compensate for the top two bits of a */

    if (top2b & 01) {
        l ^= b << 30;
        h ^= b >> 2;
    }
    if (top2b & 02) {
        l ^= b << 31;
        h ^= b >> 1;
    }

    *r1 = h;
    *r0 = l;
}