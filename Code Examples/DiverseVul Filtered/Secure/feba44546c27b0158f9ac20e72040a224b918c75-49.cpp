gdImagePtr gdImageRotate180 (gdImagePtr src)
{
	int uY, uX;
	int c;
	gdImagePtr dst;
	typedef int (*FuncPtr)(gdImagePtr, int, int);
	FuncPtr f;

	if (src->trueColor) {
		dst = gdImageCreateTrueColor ( src->sx,src->sy);
		f = gdImageGetTrueColorPixel;
	} else {
		dst = gdImageCreate (src->sx, src->sy);
		f = gdImageGetPixel;
	}

	if (dst != NULL) {
		gdImagePaletteCopy (dst, src);
		
		for (uY = 0; uY<src->sy; uY++) {
			for (uX = 0; uX<src->sx; uX++) {
				c = f (src, uX, uY);
				gdImageSetPixel(dst, (dst->sx - uX - 1), (dst->sy - uY - 1), c);
			}
		}
	}

	return dst;
}