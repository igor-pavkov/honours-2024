int gdImageNegate(gdImagePtr src)
{
	int x, y;
	int r,g,b,a;
	int new_pxl, pxl;
	typedef int (*FuncPtr)(gdImagePtr, int, int);
	FuncPtr f;

	if (src==NULL) {
		return 0;
	}
    
	f = GET_PIXEL_FUNCTION(src);

	for (y=0; y<src->sy; ++y) {
		for (x=0; x<src->sx; ++x) {
			pxl = f (src, x, y);
			r = gdImageRed(src, pxl);
			g = gdImageGreen(src, pxl);
			b = gdImageBlue(src, pxl);
			a = gdImageAlpha(src, pxl);
			
			new_pxl = gdImageColorAllocateAlpha(src, 255-r, 255-g, 255-b, a);
			if (new_pxl == -1) {
				new_pxl = gdImageColorClosestAlpha(src, 255-r, 255-g, 255-b, a);
			}
			if ((y >= 0) && (y < src->sy)) {
				gdImageSetPixel (src, x, y, new_pxl);
			}
		}
	}
	return 1;
}