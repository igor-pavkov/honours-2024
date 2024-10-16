static void select_delta_tables(TrueMotion1Context *s, int delta_table_index)
{
    int i;

    if (delta_table_index > 3)
        return;

    memcpy(s->ydt, ydts[delta_table_index], 8 * sizeof(int16_t));
    memcpy(s->cdt, cdts[delta_table_index], 8 * sizeof(int16_t));
    memcpy(s->fat_ydt, fat_ydts[delta_table_index], 8 * sizeof(int16_t));
    memcpy(s->fat_cdt, fat_cdts[delta_table_index], 8 * sizeof(int16_t));

    /* Y skinny deltas need to be halved for some reason; maybe the
     * skinny Y deltas should be modified */
    for (i = 0; i < 8; i++)
    {
        /* drop the lsb before dividing by 2-- net effect: round down
         * when dividing a negative number (e.g., -3/2 = -2, not -1) */
        s->ydt[i] &= 0xFFFE;
        s->ydt[i] /= 2;
    }
}