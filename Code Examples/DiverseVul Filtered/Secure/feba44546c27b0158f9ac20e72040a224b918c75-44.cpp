gdImagePtr gdImageRotate90 (gdImagePtr src)
{
	int uY, uX;
	int c;
	gdImagePtr dst;
	typedef int (*FuncPtr)(gdImagePtr, int, int);
	FuncPtr f;

	if (src->trueColor) {
		dst = gdImageCreateTrueColor(src->sy, src->sx);
		f = gdImageGetTrueColorPixel;
	} else {
		dst = gdImageCreate (src->sy, src->sx);
		f = gdImageGetPixel;
	}

	if (dst != NULL) {
		gdImagePaletteCopy (dst, src);
		
		for (uY = 0; uY<src->sy; uY++) {
			for (uX = 0; uX<src->sx; uX++) {
				c = f (src, uX, uY);
				gdImageSetPixel(dst, uY, (dst->sy - uX - 1), c);
			}
		}
	}
	
	return dst;
}