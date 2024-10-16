static av_always_inline void dnxhd_10bit_get_pixels_8x4_sym(int16_t *av_restrict block, const uint8_t *pixels, int line_size)
{
    int i;
    const uint16_t* pixels16 = (const uint16_t*)pixels;
    line_size >>= 1;

    for (i = 0; i < 4; i++) {
        block[0] = pixels16[0]; block[1] = pixels16[1];
        block[2] = pixels16[2]; block[3] = pixels16[3];
        block[4] = pixels16[4]; block[5] = pixels16[5];
        block[6] = pixels16[6]; block[7] = pixels16[7];
        pixels16 += line_size;
        block += 8;
    }
    memcpy(block,      block -  8, sizeof(*block) * 8);
    memcpy(block +  8, block - 16, sizeof(*block) * 8);
    memcpy(block + 16, block - 24, sizeof(*block) * 8);
    memcpy(block + 24, block - 32, sizeof(*block) * 8);
}