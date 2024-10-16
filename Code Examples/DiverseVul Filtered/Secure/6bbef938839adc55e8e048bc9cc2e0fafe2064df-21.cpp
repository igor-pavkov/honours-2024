static inline int get_block_rate(MpegEncContext *s, int16_t block[64],
                                 int block_last_index, uint8_t scantable[64])
{
    int last = 0;
    int j;
    int rate = 0;

    for (j = 1; j <= block_last_index; j++) {
        const int index = scantable[j];
        int level = block[index];
        if (level) {
            level += 64;
            if ((level & (~127)) == 0) {
                if (j < block_last_index)
                    rate += s->intra_ac_vlc_length[UNI_AC_ENC_INDEX(j - last - 1, level)];
                else
                    rate += s->intra_ac_vlc_last_length[UNI_AC_ENC_INDEX(j - last - 1, level)];
            } else
                rate += s->ac_esc_length;

            last = j;
        }
    }

    return rate;
}