static int mpeg4_get_block_length(MpegEncContext *s,
                                  int16_t *block, int n,
                                  int intra_dc, uint8_t *scan_table)
{
    int i, last_non_zero;
    uint8_t *len_tab;
    const int last_index = s->block_last_index[n];
    int len = 0;

    if (s->mb_intra) {  // Note gcc (3.2.1 at least) will optimize this away
        /* mpeg4 based DC predictor */
        len += mpeg4_get_dc_length(intra_dc, n);
        if (last_index < 1)
            return len;
        i = 1;
        len_tab = uni_mpeg4_intra_rl_len;
    } else {
        if (last_index < 0)
            return 0;
        i = 0;
        len_tab = uni_mpeg4_inter_rl_len;
    }

    /* AC coefs */
    last_non_zero = i - 1;
    for (; i < last_index; i++) {
        int level = block[scan_table[i]];
        if (level) {
            int run = i - last_non_zero - 1;
            level += 64;
            if ((level & (~127)) == 0) {
                const int index = UNI_MPEG4_ENC_INDEX(0, run, level);
                len += len_tab[index];
            } else {  // ESC3
                len += 7 + 2 + 1 + 6 + 1 + 12 + 1;
            }
            last_non_zero = i;
        }
    }
    /* if (i <= last_index) */ {
        int level = block[scan_table[i]];
        int run   = i - last_non_zero - 1;
        level += 64;
        if ((level & (~127)) == 0) {
            const int index = UNI_MPEG4_ENC_INDEX(1, run, level);
            len += len_tab[index];
        } else {  // ESC3
            len += 7 + 2 + 1 + 6 + 1 + 12 + 1;
        }
    }

    return len;
}