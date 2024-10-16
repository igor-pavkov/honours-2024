static void gen_vector_table24(TrueMotion1Context *s, const uint8_t *sel_vector_table)
{
    int len, i, j;
    unsigned char delta_pair;

    for (i = 0; i < 1024; i += 4)
    {
        len = *sel_vector_table++ / 2;
        for (j = 0; j < len; j++)
        {
            delta_pair = *sel_vector_table++;
            s->y_predictor_table[i+j] = 0xfffffffe &
                make_ydt24_entry(delta_pair >> 4, delta_pair & 0xf, s->ydt);
            s->c_predictor_table[i+j] = 0xfffffffe &
                make_cdt24_entry(delta_pair >> 4, delta_pair & 0xf, s->cdt);
            s->fat_y_predictor_table[i+j] = 0xfffffffe &
                make_ydt24_entry(delta_pair >> 4, delta_pair & 0xf, s->fat_ydt);
            s->fat_c_predictor_table[i+j] = 0xfffffffe &
                make_cdt24_entry(delta_pair >> 4, delta_pair & 0xf, s->fat_cdt);
        }
        s->y_predictor_table[i+(j-1)] |= 1;
        s->c_predictor_table[i+(j-1)] |= 1;
        s->fat_y_predictor_table[i+(j-1)] |= 1;
        s->fat_c_predictor_table[i+(j-1)] |= 1;
    }
}