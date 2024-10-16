void ff_clean_mpeg4_qscales(MpegEncContext *s)
{
    int i;
    int8_t *const qscale_table = s->current_picture.qscale_table;

    ff_clean_h263_qscales(s);

    if (s->pict_type == AV_PICTURE_TYPE_B) {
        int odd = 0;
        /* ok, come on, this isn't funny anymore, there's more code for
         * handling this mpeg4 mess than for the actual adaptive quantization */

        for (i = 0; i < s->mb_num; i++) {
            int mb_xy = s->mb_index2xy[i];
            odd += qscale_table[mb_xy] & 1;
        }

        if (2 * odd > s->mb_num)
            odd = 1;
        else
            odd = 0;

        for (i = 0; i < s->mb_num; i++) {
            int mb_xy = s->mb_index2xy[i];
            if ((qscale_table[mb_xy] & 1) != odd)
                qscale_table[mb_xy]++;
            if (qscale_table[mb_xy] > 31)
                qscale_table[mb_xy] = 31;
        }

        for (i = 1; i < s->mb_num; i++) {
            int mb_xy = s->mb_index2xy[i];
            if (qscale_table[mb_xy] != qscale_table[s->mb_index2xy[i - 1]] &&
                (s->mb_type[mb_xy] & CANDIDATE_MB_TYPE_DIRECT)) {
                s->mb_type[mb_xy] |= CANDIDATE_MB_TYPE_BIDIR;
            }
        }
    }
}