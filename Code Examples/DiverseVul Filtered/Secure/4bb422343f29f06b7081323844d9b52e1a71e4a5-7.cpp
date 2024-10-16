static inline void _gdScaleCol (gdImagePtr pSrc,  unsigned int src_width, gdImagePtr pRes, unsigned int dst_width, unsigned int dst_height, unsigned int uCol, LineContribType *contrib)
{
	unsigned int y;
	for (y = 0; y < dst_height - 1; y++) {
		register unsigned char r = 0, g = 0, b = 0, a = 0;
		const int iLeft = contrib->ContribRow[y].Left;
		const int iRight = contrib->ContribRow[y].Right;
		int i;
		int *row = pRes->tpixels[y];

		/* Accumulate each channel */
		for (i = iLeft; i <= iRight; i++) {
			const int pCurSrc = pSrc->tpixels[i][uCol];
			const int i_iLeft = i - iLeft;
			r += (unsigned char)(contrib->ContribRow[y].Weights[i_iLeft] * (double)(gdTrueColorGetRed(pCurSrc)));
			g += (unsigned char)(contrib->ContribRow[y].Weights[i_iLeft] * (double)(gdTrueColorGetGreen(pCurSrc)));
			b += (unsigned char)(contrib->ContribRow[y].Weights[i_iLeft] * (double)(gdTrueColorGetBlue(pCurSrc)));
			a += (unsigned char)(contrib->ContribRow[y].Weights[i_iLeft] * (double)(gdTrueColorGetAlpha(pCurSrc)));
		}
		pRes->tpixels[y][uCol] = gdTrueColorAlpha(r, g, b, a);
	}
}