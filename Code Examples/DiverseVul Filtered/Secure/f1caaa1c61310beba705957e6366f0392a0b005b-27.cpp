static av_always_inline int dnxhd_ssd_block(int16_t *qblock, int16_t *block)
{
    int score = 0;
    int i;
    for (i = 0; i < 64; i++)
        score += (block[i] - qblock[i]) * (block[i] - qblock[i]);
    return score;
}