int RSAZ_mod_exp_x2_ifma256(BN_ULONG *out,
                            const BN_ULONG *base,
                            const BN_ULONG *exp[2],
                            const BN_ULONG *m,
                            const BN_ULONG *rr,
                            const BN_ULONG k0[2],
                            int modulus_bitsize)
{
    typedef void (*DAMM)(BN_ULONG *res, const BN_ULONG *a,
                         const BN_ULONG *b, const BN_ULONG *m,
                         const BN_ULONG k0[2]);
    typedef void (*DEXTRACT)(BN_ULONG *res, const BN_ULONG *red_table,
                             int red_table_idx, int tbl_idx);

    int ret = 0;
    int idx;

    /* Exponent window size */
    int exp_win_size = 5;
    int exp_win_mask = (1U << exp_win_size) - 1;

    /*
    * Number of digits (64-bit words) in redundant representation to handle
    * modulus bits
    */
    int red_digits = 0;
    int exp_digits = 0;

    BN_ULONG *storage = NULL;
    BN_ULONG *storage_aligned = NULL;
    int storage_len_bytes = 0;

    /* Red(undant) result Y and multiplier X */
    BN_ULONG *red_Y = NULL;     /* [2][red_digits] */
    BN_ULONG *red_X = NULL;     /* [2][red_digits] */
    /* Pre-computed table of base powers */
    BN_ULONG *red_table = NULL; /* [1U << exp_win_size][2][red_digits] */
    /* Expanded exponent */
    BN_ULONG *expz = NULL;      /* [2][exp_digits + 1] */

    /* Dual AMM */
    DAMM damm = NULL;
    /* Extractor from red_table */
    DEXTRACT extract = NULL;

/*
 * Squaring is done using multiplication now. That can be a subject of
 * optimization in future.
 */
# define DAMS(r,a,m,k0) damm((r),(a),(a),(m),(k0))

    switch (modulus_bitsize) {
    case 1024:
        red_digits = 20;
        exp_digits = 16;
        damm = ossl_rsaz_amm52x20_x2_ifma256;
        extract = ossl_extract_multiplier_2x20_win5;
        break;
    case 1536:
        /* Extended with 2 digits padding to avoid mask ops in high YMM register */
        red_digits = 30 + 2;
        exp_digits = 24;
        damm = ossl_rsaz_amm52x30_x2_ifma256;
        extract = ossl_extract_multiplier_2x30_win5;
        break;
    case 2048:
        red_digits = 40;
        exp_digits = 32;
        damm = ossl_rsaz_amm52x40_x2_ifma256;
        extract = ossl_extract_multiplier_2x40_win5;
        break;
    default:
        goto err;
    }

    storage_len_bytes = (2 * red_digits                         /* red_Y     */
                       + 2 * red_digits                         /* red_X     */
                       + 2 * red_digits * (1U << exp_win_size)  /* red_table */
                       + 2 * (exp_digits + 1))                  /* expz      */
                       * sizeof(BN_ULONG)
                       + 64;                                    /* alignment */

    storage = (BN_ULONG *)OPENSSL_zalloc(storage_len_bytes);
    if (storage == NULL)
        goto err;
    storage_aligned = (BN_ULONG *)ALIGN_OF(storage, 64);

    red_Y     = storage_aligned;
    red_X     = red_Y + 2 * red_digits;
    red_table = red_X + 2 * red_digits;
    expz      = red_table + 2 * red_digits * (1U << exp_win_size);

    /*
     * Compute table of powers base^i, i = 0, ..., (2^EXP_WIN_SIZE) - 1
     *   table[0] = mont(x^0) = mont(1)
     *   table[1] = mont(x^1) = mont(x)
     */
    red_X[0 * red_digits] = 1;
    red_X[1 * red_digits] = 1;
    damm(&red_table[0 * 2 * red_digits], (const BN_ULONG*)red_X, rr, m, k0);
    damm(&red_table[1 * 2 * red_digits], base,  rr, m, k0);

    for (idx = 1; idx < (int)((1U << exp_win_size) / 2); idx++) {
        DAMS(&red_table[(2 * idx + 0) * 2 * red_digits],
             &red_table[(1 * idx)     * 2 * red_digits], m, k0);
        damm(&red_table[(2 * idx + 1) * 2 * red_digits],
             &red_table[(2 * idx)     * 2 * red_digits],
             &red_table[1 * 2 * red_digits], m, k0);
    }

    /* Copy and expand exponents */
    memcpy(&expz[0 * (exp_digits + 1)], exp[0], exp_digits * sizeof(BN_ULONG));
    expz[1 * (exp_digits + 1) - 1] = 0;
    memcpy(&expz[1 * (exp_digits + 1)], exp[1], exp_digits * sizeof(BN_ULONG));
    expz[2 * (exp_digits + 1) - 1] = 0;

    /* Exponentiation */
    {
        const int rem = modulus_bitsize % exp_win_size;
        const BN_ULONG table_idx_mask = exp_win_mask;

        int exp_bit_no = modulus_bitsize - rem;
        int exp_chunk_no = exp_bit_no / 64;
        int exp_chunk_shift = exp_bit_no % 64;

        BN_ULONG red_table_idx_0, red_table_idx_1;

        /*
         * If rem == 0, then
         *      exp_bit_no = modulus_bitsize - exp_win_size
         * However, this isn't possible because rem is { 1024, 1536, 2048 } % 5
         * which is { 4, 1, 3 } respectively.
         *
         * If this assertion ever fails the fix above is easy.
         */
        OPENSSL_assert(rem != 0);

        /* Process 1-st exp window - just init result */
        red_table_idx_0 = expz[exp_chunk_no + 0 * (exp_digits + 1)];
        red_table_idx_1 = expz[exp_chunk_no + 1 * (exp_digits + 1)];

        /*
         * The function operates with fixed moduli sizes divisible by 64,
         * thus table index here is always in supported range [0, EXP_WIN_SIZE).
         */
        red_table_idx_0 >>= exp_chunk_shift;
        red_table_idx_1 >>= exp_chunk_shift;

        extract(&red_Y[0 * red_digits], (const BN_ULONG*)red_table, (int)red_table_idx_0, (int)red_table_idx_1);

        /* Process other exp windows */
        for (exp_bit_no -= exp_win_size; exp_bit_no >= 0; exp_bit_no -= exp_win_size) {
            /* Extract pre-computed multiplier from the table */
            {
                BN_ULONG T;

                exp_chunk_no = exp_bit_no / 64;
                exp_chunk_shift = exp_bit_no % 64;
                {
                    red_table_idx_0 = expz[exp_chunk_no + 0 * (exp_digits + 1)];
                    T = expz[exp_chunk_no + 1 + 0 * (exp_digits + 1)];

                    red_table_idx_0 >>= exp_chunk_shift;
                    /*
                     * Get additional bits from then next quadword
                     * when 64-bit boundaries are crossed.
                     */
                    if (exp_chunk_shift > 64 - exp_win_size) {
                        T <<= (64 - exp_chunk_shift);
                        red_table_idx_0 ^= T;
                    }
                    red_table_idx_0 &= table_idx_mask;
                }
                {
                    red_table_idx_1 = expz[exp_chunk_no + 1 * (exp_digits + 1)];
                    T = expz[exp_chunk_no + 1 + 1 * (exp_digits + 1)];

                    red_table_idx_1 >>= exp_chunk_shift;
                    /*
                     * Get additional bits from then next quadword
                     * when 64-bit boundaries are crossed.
                     */
                    if (exp_chunk_shift > 64 - exp_win_size) {
                        T <<= (64 - exp_chunk_shift);
                        red_table_idx_1 ^= T;
                    }
                    red_table_idx_1 &= table_idx_mask;
                }

                extract(&red_X[0 * red_digits], (const BN_ULONG*)red_table, (int)red_table_idx_0, (int)red_table_idx_1);
            }

            /* Series of squaring */
            DAMS((BN_ULONG*)red_Y, (const BN_ULONG*)red_Y, m, k0);
            DAMS((BN_ULONG*)red_Y, (const BN_ULONG*)red_Y, m, k0);
            DAMS((BN_ULONG*)red_Y, (const BN_ULONG*)red_Y, m, k0);
            DAMS((BN_ULONG*)red_Y, (const BN_ULONG*)red_Y, m, k0);
            DAMS((BN_ULONG*)red_Y, (const BN_ULONG*)red_Y, m, k0);

            damm((BN_ULONG*)red_Y, (const BN_ULONG*)red_Y, (const BN_ULONG*)red_X, m, k0);
        }
    }

    /*
     *
     * NB: After the last AMM of exponentiation in Montgomery domain, the result
     * may be (modulus_bitsize + 1), but the conversion out of Montgomery domain
     * performs an AMM(x,1) which guarantees that the final result is less than
     * |m|, so no conditional subtraction is needed here. See [1] for details.
     *
     * [1] Gueron, S. Efficient software implementations of modular exponentiation.
     *     DOI: 10.1007/s13389-012-0031-5
     */

    /* Convert result back in regular 2^52 domain */
    memset(red_X, 0, 2 * red_digits * sizeof(BN_ULONG));
    red_X[0 * red_digits] = 1;
    red_X[1 * red_digits] = 1;
    damm(out, (const BN_ULONG*)red_Y, (const BN_ULONG*)red_X, m, k0);

    ret = 1;

err:
    if (storage != NULL) {
        /* Clear whole storage */
        OPENSSL_cleanse(storage, storage_len_bytes);
        OPENSSL_free(storage);
    }

#undef DAMS
    return ret;
}