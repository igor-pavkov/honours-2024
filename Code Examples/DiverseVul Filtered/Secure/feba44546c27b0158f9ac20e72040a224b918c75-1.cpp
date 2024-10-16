int gdImageContrast(gdImagePtr src, double contrast)
{
	int x, y;
	int r,g,b,a;
	double rf,gf,bf;
	int new_pxl, pxl;
	typedef int (*FuncPtr)(gdImagePtr, int, int);

	FuncPtr f;
	f = GET_PIXEL_FUNCTION(src);

	if (src==NULL) {
		return 0;
	}
	
	contrast = (double)(100.0-contrast)/100.0;
	contrast = contrast*contrast;
	
	for (y=0; y<src->sy; ++y) {
		for (x=0; x<src->sx; ++x) {
			pxl = f(src, x, y); 
			
			r = gdImageRed(src, pxl);
			g = gdImageGreen(src, pxl);
			b = gdImageBlue(src, pxl);
			a = gdImageAlpha(src, pxl);

			rf = (double)r/255.0;
			rf = rf-0.5;
			rf = rf*contrast;
			rf = rf+0.5;
			rf = rf*255.0;

			bf = (double)b/255.0;
			bf = bf-0.5;
			bf = bf*contrast;
			bf = bf+0.5;
			bf = bf*255.0;
      
			gf = (double)g/255.0;
			gf = gf-0.5;
			gf = gf*contrast;
			gf = gf+0.5;
			gf = gf*255.0;

			rf = (rf > 255.0)? 255.0 : ((rf < 0.0)? 0.0:rf);
			gf = (gf > 255.0)? 255.0 : ((gf < 0.0)? 0.0:gf);
			bf = (bf > 255.0)? 255.0 : ((bf < 0.0)? 0.0:bf);

			new_pxl = gdImageColorAllocateAlpha(src, (int)rf, (int)gf, (int)bf, a);
			if (new_pxl == -1) {
				new_pxl = gdImageColorClosestAlpha(src, (int)rf, (int)gf, (int)bf, a);
			}
			if ((y >= 0) && (y < src->sy)) {
				gdImageSetPixel (src, x, y, new_pxl);
			}
		}
	}
	return 1;
}