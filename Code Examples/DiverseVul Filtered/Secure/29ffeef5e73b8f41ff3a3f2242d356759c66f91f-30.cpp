static void init_dequant8_coeff_table(H264Context *h)
{
    int i, j, q, x;
    const int max_qp = 51 + 6 * (h->sps.bit_depth_luma - 8);

    for (i = 0; i < 6; i++) {
        h->dequant8_coeff[i] = h->dequant8_buffer[i];
        for (j = 0; j < i; j++)
            if (!memcmp(h->pps.scaling_matrix8[j], h->pps.scaling_matrix8[i],
                        64 * sizeof(uint8_t))) {
                h->dequant8_coeff[i] = h->dequant8_buffer[j];
                break;
            }
        if (j < i)
            continue;

        for (q = 0; q < max_qp + 1; q++) {
            int shift = div6[q];
            int idx   = rem6[q];
            for (x = 0; x < 64; x++)
                h->dequant8_coeff[i][q][(x >> 3) | ((x & 7) << 3)] =
                    ((uint32_t)dequant8_coeff_init[idx][dequant8_coeff_init_scan[((x >> 1) & 12) | (x & 3)]] *
                     h->pps.scaling_matrix8[i][x]) << shift;
        }
    }
}