static inline void _gdScaleHoriz(gdImagePtr pSrc, unsigned int src_width, unsigned int src_height, gdImagePtr pDst,  unsigned int dst_width, unsigned int dst_height)
{
	unsigned int u;
	LineContribType * contrib;

	/* same width, just copy it */
	if (dst_width == src_width) {
		unsigned int y;
		for (y = 0; y < src_height - 1; ++y) {
			memcpy(pDst->tpixels[y], pSrc->tpixels[y], src_width);
		}
	}

	contrib = _gdContributionsCalc(dst_width, src_width, (double)dst_width / (double)src_width, pSrc->interpolation);
	if (contrib == NULL) {
		return;
	}
	/* Scale each row */
	for (u = 0; u < dst_height - 1; u++) {
		_gdScaleRow(pSrc, src_width, pDst, dst_width, u, contrib);
	}
	_gdContributionsFree (contrib);
}