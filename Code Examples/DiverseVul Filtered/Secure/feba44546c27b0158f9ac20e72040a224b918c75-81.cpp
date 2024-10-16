int gdImageBrightness(gdImagePtr src, int brightness)
{
	int x, y;
	int r,g,b,a;
	int new_pxl, pxl;
	typedef int (*FuncPtr)(gdImagePtr, int, int);
	FuncPtr f;
	f = GET_PIXEL_FUNCTION(src);

	if (src==NULL || (brightness < -255 || brightness>255)) {
		return 0;
	}
  
	if (brightness==0) {
		return 1;
	}

	for (y=0; y<src->sy; ++y) {
		for (x=0; x<src->sx; ++x) {
			pxl = f (src, x, y);
			
			r = gdImageRed(src, pxl);
			g = gdImageGreen(src, pxl);
			b = gdImageBlue(src, pxl);
			a = gdImageAlpha(src, pxl);
			
			r = r + brightness;
			g = g + brightness;
			b = b + brightness;
			
			r = (r > 255)? 255 : ((r < 0)? 0:r);
			g = (g > 255)? 255 : ((g < 0)? 0:g);
			b = (b > 255)? 255 : ((b < 0)? 0:b);

			new_pxl = gdImageColorAllocateAlpha(src, (int)r, (int)g, (int)b, a);
			if (new_pxl == -1) {
				new_pxl = gdImageColorClosestAlpha(src, (int)r, (int)g, (int)b, a);
			}
			if ((y >= 0) && (y < src->sy)) {
				gdImageSetPixel (src, x, y, new_pxl);
			}
		}
	}
	return 1;
}