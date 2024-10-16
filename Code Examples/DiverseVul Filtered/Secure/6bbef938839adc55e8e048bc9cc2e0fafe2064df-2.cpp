static inline int decide_ac_pred(MpegEncContext *s, int16_t block[6][64],
                                 const int dir[6], uint8_t *st[6],
                                 int zigzag_last_index[6])
{
    int score = 0;
    int i, n;
    int8_t *const qscale_table = s->current_picture.qscale_table;

    memcpy(zigzag_last_index, s->block_last_index, sizeof(int) * 6);

    for (n = 0; n < 6; n++) {
        int16_t *ac_val, *ac_val1;

        score -= get_block_rate(s, block[n], s->block_last_index[n],
                                s->intra_scantable.permutated);

        ac_val  = s->ac_val[0][0] + s->block_index[n] * 16;
        ac_val1 = ac_val;
        if (dir[n]) {
            const int xy = s->mb_x + s->mb_y * s->mb_stride - s->mb_stride;
            /* top prediction */
            ac_val -= s->block_wrap[n] * 16;
            if (s->mb_y == 0 || s->qscale == qscale_table[xy] || n == 2 || n == 3) {
                /* same qscale */
                for (i = 1; i < 8; i++) {
                    const int level = block[n][s->idsp.idct_permutation[i]];
                    block[n][s->idsp.idct_permutation[i]] = level - ac_val[i + 8];
                    ac_val1[i]     = block[n][s->idsp.idct_permutation[i << 3]];
                    ac_val1[i + 8] = level;
                }
            } else {
                /* different qscale, we must rescale */
                for (i = 1; i < 8; i++) {
                    const int level = block[n][s->idsp.idct_permutation[i]];
                    block[n][s->idsp.idct_permutation[i]] = level - ROUNDED_DIV(ac_val[i + 8] * qscale_table[xy], s->qscale);
                    ac_val1[i]     = block[n][s->idsp.idct_permutation[i << 3]];
                    ac_val1[i + 8] = level;
                }
            }
            st[n] = s->intra_h_scantable.permutated;
        } else {
            const int xy = s->mb_x - 1 + s->mb_y * s->mb_stride;
            /* left prediction */
            ac_val -= 16;
            if (s->mb_x == 0 || s->qscale == qscale_table[xy] || n == 1 || n == 3) {
                /* same qscale */
                for (i = 1; i < 8; i++) {
                    const int level = block[n][s->idsp.idct_permutation[i << 3]];
                    block[n][s->idsp.idct_permutation[i << 3]] = level - ac_val[i];
                    ac_val1[i]     = level;
                    ac_val1[i + 8] = block[n][s->idsp.idct_permutation[i]];
                }
            } else {
                /* different qscale, we must rescale */
                for (i = 1; i < 8; i++) {
                    const int level = block[n][s->idsp.idct_permutation[i << 3]];
                    block[n][s->idsp.idct_permutation[i << 3]] = level - ROUNDED_DIV(ac_val[i] * qscale_table[xy], s->qscale);
                    ac_val1[i]     = level;
                    ac_val1[i + 8] = block[n][s->idsp.idct_permutation[i]];
                }
            }
            st[n] = s->intra_v_scantable.permutated;
        }

        for (i = 63; i > 0; i--)  // FIXME optimize
            if (block[n][st[n][i]])
                break;
        s->block_last_index[n] = i;

        score += get_block_rate(s, block[n], s->block_last_index[n], st[n]);
    }

    if (score < 0) {
        return 1;
    } else {
        restore_ac_coeffs(s, block, dir, st, zigzag_last_index);
        return 0;
    }
}