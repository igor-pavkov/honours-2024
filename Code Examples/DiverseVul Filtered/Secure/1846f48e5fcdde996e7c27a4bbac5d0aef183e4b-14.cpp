BGD_DECLARE(void) gdImageCopyResampled (gdImagePtr dst,
										gdImagePtr src,
										int dstX, int dstY,
										int srcX, int srcY,
										int dstW, int dstH, int srcW, int srcH)
{
	int x, y;
	if (!dst->trueColor) {
		gdImageCopyResized (dst, src, dstX, dstY, srcX, srcY, dstW, dstH, srcW, srcH);
		return;
	}
	for (y = dstY; (y < dstY + dstH); y++) {
		for (x = dstX; (x < dstX + dstW); x++) {
			float sy1, sy2, sx1, sx2;
			float sx, sy;
			float spixels = 0.0;
			float red = 0.0, green = 0.0, blue = 0.0, alpha = 0.0;
			float alpha_factor, alpha_sum = 0.0, contrib_sum = 0.0;
			sy1 = ((float)(y - dstY)) * (float)srcH / (float)dstH;
			sy2 = ((float)(y + 1 - dstY)) * (float) srcH / (float) dstH;
			sy = sy1;
			do {
				float yportion;
				if (floorf(sy) == floorf(sy1)) {
					yportion = 1.0 - (sy - floorf(sy));
					if (yportion > sy2 - sy1) {
						yportion = sy2 - sy1;
					}
					sy = floorf(sy);
				} else if (sy == floorf(sy2)) {
					yportion = sy2 - floorf(sy2);
				} else {
					yportion = 1.0;
				}
				sx1 = ((float)(x - dstX)) * (float) srcW / dstW;
				sx2 = ((float)(x + 1 - dstX)) * (float) srcW / dstW;
				sx = sx1;
				do {
					float xportion;
					float pcontribution;
					int p;
					if (floorf(sx) == floorf(sx1)) {
						xportion = 1.0 - (sx - floorf(sx));
						if (xportion > sx2 - sx1) {
							xportion = sx2 - sx1;
						}
						sx = floorf(sx);
					} else if (sx == floorf(sx2)) {
						xportion = sx2 - floorf(sx2);
					} else {
						xportion = 1.0;
					}
					pcontribution = xportion * yportion;
					p = gdImageGetTrueColorPixel(src, (int) sx + srcX, (int) sy + srcY);

					alpha_factor = ((gdAlphaMax - gdTrueColorGetAlpha(p))) * pcontribution;
					red += gdTrueColorGetRed (p) * alpha_factor;
					green += gdTrueColorGetGreen (p) * alpha_factor;
					blue += gdTrueColorGetBlue (p) * alpha_factor;
					alpha += gdTrueColorGetAlpha (p) * pcontribution;
					alpha_sum += alpha_factor;
					contrib_sum += pcontribution;
					spixels += xportion * yportion;
					sx += 1.0;
				}
				while (sx < sx2);
				sy += 1.0f;
			}
			while (sy < sy2);

			if (spixels != 0.0) {
				red /= spixels;
				green /= spixels;
				blue /= spixels;
				alpha /= spixels;
			}
			if ( alpha_sum != 0.0) {
				if( contrib_sum != 0.0) {
					alpha_sum /= contrib_sum;
				}
				red /= alpha_sum;
				green /= alpha_sum;
				blue /= alpha_sum;
			}
			/* Clamping to allow for rounding errors above */
			if (red > 255.0) {
				red = 255.0;
			}
			if (green > 255.0) {
				green = 255.0;
			}
			if (blue > 255.0f) {
				blue = 255.0;
			}
			if (alpha > gdAlphaMax) {
				alpha = gdAlphaMax;
			}
			gdImageSetPixel(dst, x, y, gdTrueColorAlpha ((int) red, (int) green, (int) blue, (int) alpha));
		}
	}
}