static void init_dequant4_coeff_table(H264Context *h)
{
    int i, j, q, x;
    const int max_qp = 51 + 6 * (h->sps.bit_depth_luma - 8);
    for (i = 0; i < 6; i++) {
        h->dequant4_coeff[i] = h->dequant4_buffer[i];
        for (j = 0; j < i; j++)
            if (!memcmp(h->pps.scaling_matrix4[j], h->pps.scaling_matrix4[i],
                        16 * sizeof(uint8_t))) {
                h->dequant4_coeff[i] = h->dequant4_buffer[j];
                break;
            }
        if (j < i)
            continue;

        for (q = 0; q < max_qp + 1; q++) {
            int shift = div6[q] + 2;
            int idx   = rem6[q];
            for (x = 0; x < 16; x++)
                h->dequant4_coeff[i][q][(x >> 2) | ((x << 2) & 0xF)] =
                    ((uint32_t)dequant4_coeff_init[idx][(x & 1) + ((x >> 2) & 1)] *
                     h->pps.scaling_matrix4[i][x]) << shift;
        }
    }
}