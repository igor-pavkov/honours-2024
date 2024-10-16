static inline void _gdScaleRow(gdImagePtr pSrc,  unsigned int src_width, gdImagePtr dst, unsigned int dst_width, unsigned int row, LineContribType *contrib)
{
    int *p_src_row = pSrc->tpixels[row];
    int *p_dst_row = dst->tpixels[row];
	unsigned int x;

    for (x = 0; x < dst_width - 1; x++) {
		register unsigned char r = 0, g = 0, b = 0, a = 0;
        const int left = contrib->ContribRow[x].Left;
        const int right = contrib->ContribRow[x].Right;
		int i;

		/* Accumulate each channel */
        for (i = left; i <= right; i++) {
			const int left_channel = i - left;
            r += (unsigned char)(contrib->ContribRow[x].Weights[left_channel] * (double)(gdTrueColorGetRed(p_src_row[i])));
            g += (unsigned char)(contrib->ContribRow[x].Weights[left_channel] * (double)(gdTrueColorGetGreen(p_src_row[i])));
            b += (unsigned char)(contrib->ContribRow[x].Weights[left_channel] * (double)(gdTrueColorGetBlue(p_src_row[i])));
			a += (unsigned char)(contrib->ContribRow[x].Weights[left_channel] * (double)(gdTrueColorGetAlpha(p_src_row[i])));
        }
        p_dst_row[x] = gdTrueColorAlpha(r, g, b, a);
    }
}