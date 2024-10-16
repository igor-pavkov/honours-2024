static void dnxhd_8bit_get_pixels_8x4_sym(int16_t *av_restrict block, const uint8_t *pixels, int line_size)
{
    int i;
    for (i = 0; i < 4; i++) {
        block[0] = pixels[0]; block[1] = pixels[1];
        block[2] = pixels[2]; block[3] = pixels[3];
        block[4] = pixels[4]; block[5] = pixels[5];
        block[6] = pixels[6]; block[7] = pixels[7];
        pixels += line_size;
        block += 8;
    }
    memcpy(block,      block -  8, sizeof(*block) * 8);
    memcpy(block +  8, block - 16, sizeof(*block) * 8);
    memcpy(block + 16, block - 24, sizeof(*block) * 8);
    memcpy(block + 24, block - 32, sizeof(*block) * 8);
}