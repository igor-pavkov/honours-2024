static av_cold void init_uni_mpeg4_rl_tab(RLTable *rl, uint32_t *bits_tab,
                                          uint8_t *len_tab)
{
    int slevel, run, last;

    av_assert0(MAX_LEVEL >= 64);
    av_assert0(MAX_RUN >= 63);

    for (slevel = -64; slevel < 64; slevel++) {
        if (slevel == 0)
            continue;
        for (run = 0; run < 64; run++) {
            for (last = 0; last <= 1; last++) {
                const int index = UNI_MPEG4_ENC_INDEX(last, run, slevel + 64);
                int level       = slevel < 0 ? -slevel : slevel;
                int sign        = slevel < 0 ? 1 : 0;
                int bits, len, code;
                int level1, run1;

                len_tab[index] = 100;

                /* ESC0 */
                code = get_rl_index(rl, last, run, level);
                bits = rl->table_vlc[code][0];
                len  = rl->table_vlc[code][1];
                bits = bits * 2 + sign;
                len++;

                if (code != rl->n && len < len_tab[index]) {
                    bits_tab[index] = bits;
                    len_tab[index]  = len;
                }
                /* ESC1 */
                bits = rl->table_vlc[rl->n][0];
                len  = rl->table_vlc[rl->n][1];
                bits = bits * 2;
                len++;                 // esc1
                level1 = level - rl->max_level[last][run];
                if (level1 > 0) {
                    code   = get_rl_index(rl, last, run, level1);
                    bits <<= rl->table_vlc[code][1];
                    len   += rl->table_vlc[code][1];
                    bits  += rl->table_vlc[code][0];
                    bits   = bits * 2 + sign;
                    len++;

                    if (code != rl->n && len < len_tab[index]) {
                        bits_tab[index] = bits;
                        len_tab[index]  = len;
                    }
                }
                /* ESC2 */
                bits = rl->table_vlc[rl->n][0];
                len  = rl->table_vlc[rl->n][1];
                bits = bits * 4 + 2;
                len += 2;                 // esc2
                run1 = run - rl->max_run[last][level] - 1;
                if (run1 >= 0) {
                    code   = get_rl_index(rl, last, run1, level);
                    bits <<= rl->table_vlc[code][1];
                    len   += rl->table_vlc[code][1];
                    bits  += rl->table_vlc[code][0];
                    bits   = bits * 2 + sign;
                    len++;

                    if (code != rl->n && len < len_tab[index]) {
                        bits_tab[index] = bits;
                        len_tab[index]  = len;
                    }
                }
                /* ESC3 */
                bits = rl->table_vlc[rl->n][0];
                len  = rl->table_vlc[rl->n][1];
                bits = bits * 4 + 3;
                len += 2;                 // esc3
                bits = bits * 2 + last;
                len++;
                bits = bits * 64 + run;
                len += 6;
                bits = bits * 2 + 1;
                len++;                    // marker
                bits = bits * 4096 + (slevel & 0xfff);
                len += 12;
                bits = bits * 2 + 1;
                len++;                    // marker

                if (len < len_tab[index]) {
                    bits_tab[index] = bits;
                    len_tab[index]  = len;
                }
            }
        }
    }
}