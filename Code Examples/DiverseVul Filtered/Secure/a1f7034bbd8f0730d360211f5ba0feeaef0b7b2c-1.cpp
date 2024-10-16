static void RSAZ_exp52x20_x2_256(BN_ULONG *out,          /* [2][20] */
                                 const BN_ULONG *base,   /* [2][20] */
                                 const BN_ULONG *exp[2], /* 2x16    */
                                 const BN_ULONG *m,      /* [2][20] */
                                 const BN_ULONG *rr,     /* [2][20] */
                                 const BN_ULONG k0[2])
{
# define BITSIZE_MODULUS (1024)
# define EXP_WIN_SIZE (5)
# define EXP_WIN_MASK ((1U << EXP_WIN_SIZE) - 1)
/*
 * Number of digits (64-bit words) in redundant representation to handle
 * modulus bits
 */
# define RED_DIGITS (20)
# define EXP_DIGITS (16)
# define DAMM ossl_rsaz_amm52x20_x2_256
/*
 * Squaring is done using multiplication now. That can be a subject of
 * optimization in future.
 */
# define DAMS(r,a,m,k0) \
              ossl_rsaz_amm52x20_x2_256((r),(a),(a),(m),(k0))

    /* Allocate stack for red(undant) result Y and multiplier X */
    ALIGN64 BN_ULONG red_Y[2][RED_DIGITS];
    ALIGN64 BN_ULONG red_X[2][RED_DIGITS];

    /* Allocate expanded exponent */
    ALIGN64 BN_ULONG expz[2][EXP_DIGITS + 1];

    /* Pre-computed table of base powers */
    ALIGN64 BN_ULONG red_table[1U << EXP_WIN_SIZE][2][RED_DIGITS];

    int idx;

    memset(red_Y, 0, sizeof(red_Y));
    memset(red_table, 0, sizeof(red_table));
    memset(red_X, 0, sizeof(red_X));

    /*
     * Compute table of powers base^i, i = 0, ..., (2^EXP_WIN_SIZE) - 1
     *   table[0] = mont(x^0) = mont(1)
     *   table[1] = mont(x^1) = mont(x)
     */
    red_X[0][0] = 1;
    red_X[1][0] = 1;
    DAMM(red_table[0][0], (const BN_ULONG*)red_X, rr, m, k0);
    DAMM(red_table[1][0], base,  rr, m, k0);

    for (idx = 1; idx < (int)((1U << EXP_WIN_SIZE) / 2); idx++) {
        DAMS(red_table[2 * idx + 0][0], red_table[1 * idx][0], m, k0);
        DAMM(red_table[2 * idx + 1][0], red_table[2 * idx][0], red_table[1][0], m, k0);
    }

    /* Copy and expand exponents */
    memcpy(expz[0], exp[0], EXP_DIGITS * sizeof(BN_ULONG));
    expz[0][EXP_DIGITS] = 0;
    memcpy(expz[1], exp[1], EXP_DIGITS * sizeof(BN_ULONG));
    expz[1][EXP_DIGITS] = 0;

    /* Exponentiation */
    {
        const int rem = BITSIZE_MODULUS % EXP_WIN_SIZE;
        BN_ULONG table_idx_mask = EXP_WIN_MASK;

        int exp_bit_no = BITSIZE_MODULUS - rem;
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
        red_table_idx_0 = expz[0][exp_chunk_no];
        red_table_idx_1 = expz[1][exp_chunk_no];
        /*
         * The function operates with fixed moduli sizes divisible by 64,
         * thus table index here is always in supported range [0, EXP_WIN_SIZE).
         */
        red_table_idx_0 >>= exp_chunk_shift;
        red_table_idx_1 >>= exp_chunk_shift;

        ossl_extract_multiplier_2x20_win5(red_Y[0], (const BN_ULONG*)red_table,
                                          (int)red_table_idx_0, 0);
        ossl_extract_multiplier_2x20_win5(red_Y[1], (const BN_ULONG*)red_table,
                                          (int)red_table_idx_1, 1);

        /* Process other exp windows */
        for (exp_bit_no -= EXP_WIN_SIZE; exp_bit_no >= 0; exp_bit_no -= EXP_WIN_SIZE) {
            /* Extract pre-computed multiplier from the table */
            {
                BN_ULONG T;

                exp_chunk_no = exp_bit_no / 64;
                exp_chunk_shift = exp_bit_no % 64;
                {
                    red_table_idx_0 = expz[0][exp_chunk_no];
                    T = expz[0][exp_chunk_no + 1];

                    red_table_idx_0 >>= exp_chunk_shift;
                    /*
                     * Get additional bits from then next quadword
                     * when 64-bit boundaries are crossed.
                     */
                    if (exp_chunk_shift > 64 - EXP_WIN_SIZE) {
                        T <<= (64 - exp_chunk_shift);
                        red_table_idx_0 ^= T;
                    }
                    red_table_idx_0 &= table_idx_mask;

                    ossl_extract_multiplier_2x20_win5(red_X[0],
                                                      (const BN_ULONG*)red_table,
                                                      (int)red_table_idx_0, 0);
                }
                {
                    red_table_idx_1 = expz[1][exp_chunk_no];
                    T = expz[1][exp_chunk_no + 1];

                    red_table_idx_1 >>= exp_chunk_shift;
                    /*
                     * Get additional bits from then next quadword
                     * when 64-bit boundaries are crossed.
                     */
                    if (exp_chunk_shift > 64 - EXP_WIN_SIZE) {
                        T <<= (64 - exp_chunk_shift);
                        red_table_idx_1 ^= T;
                    }
                    red_table_idx_1 &= table_idx_mask;

                    ossl_extract_multiplier_2x20_win5(red_X[1],
                                                      (const BN_ULONG*)red_table,
                                                      (int)red_table_idx_1, 1);
                }
            }

            /* Series of squaring */
            DAMS((BN_ULONG*)red_Y, (const BN_ULONG*)red_Y, m, k0);
            DAMS((BN_ULONG*)red_Y, (const BN_ULONG*)red_Y, m, k0);
            DAMS((BN_ULONG*)red_Y, (const BN_ULONG*)red_Y, m, k0);
            DAMS((BN_ULONG*)red_Y, (const BN_ULONG*)red_Y, m, k0);
            DAMS((BN_ULONG*)red_Y, (const BN_ULONG*)red_Y, m, k0);

            DAMM((BN_ULONG*)red_Y, (const BN_ULONG*)red_Y, (const BN_ULONG*)red_X, m, k0);
        }
    }

    /*
     *
     * NB: After the last AMM of exponentiation in Montgomery domain, the result
     * may be 1025-bit, but the conversion out of Montgomery domain performs an
     * AMM(x,1) which guarantees that the final result is less than |m|, so no
     * conditional subtraction is needed here. See "Efficient Software
     * Implementations of Modular Exponentiation" (by Shay Gueron) paper for details.
     */

    /* Convert result back in regular 2^52 domain */
    memset(red_X, 0, sizeof(red_X));
    red_X[0][0] = 1;
    red_X[1][0] = 1;
    DAMM(out, (const BN_ULONG*)red_Y, (const BN_ULONG*)red_X, m, k0);

    /* Clear exponents */
    OPENSSL_cleanse(expz, sizeof(expz));
    OPENSSL_cleanse(red_Y, sizeof(red_Y));

# undef DAMS
# undef DAMM
# undef EXP_DIGITS
# undef RED_DIGITS
# undef EXP_WIN_MASK
# undef EXP_WIN_SIZE
# undef BITSIZE_MODULUS
}