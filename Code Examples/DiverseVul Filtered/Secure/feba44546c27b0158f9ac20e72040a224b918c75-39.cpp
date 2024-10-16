int gdImageConvolution(gdImagePtr src, float filter[3][3], float filter_div, float offset)
{
	int         x, y, i, j, new_a;
	float       new_r, new_g, new_b;
	int         new_pxl, pxl=0;
	gdImagePtr  srcback;
	typedef int (*FuncPtr)(gdImagePtr, int, int);
	FuncPtr f;

	if (src==NULL) {
		return 0;
	}

	/* We need the orinal image with each safe neoghb. pixel */
	srcback = gdImageCreateTrueColor (src->sx, src->sy);
	gdImageCopy(srcback, src,0,0,0,0,src->sx,src->sy);
  
	if (srcback==NULL) {
		return 0;
	}

	f = GET_PIXEL_FUNCTION(src);

	for ( y=0; y<src->sy; y++) {
		for(x=0; x<src->sx; x++) {
			new_r = new_g = new_b = 0;
			new_a = gdImageAlpha(srcback, pxl);
			
			for (j=0; j<3; j++) {
				for (i=0; i<3; i++) {
				        pxl = f(srcback, x-(3>>1)+i, y-(3>>1)+j); 
					new_r += (float)gdImageRed(srcback, pxl) * filter[j][i];
					new_g += (float)gdImageGreen(srcback, pxl) * filter[j][i];
					new_b += (float)gdImageBlue(srcback, pxl) * filter[j][i];
				}
			}	
			
			new_r = (new_r/filter_div)+offset;
			new_g = (new_g/filter_div)+offset;
			new_b = (new_b/filter_div)+offset;

			new_r = (new_r > 255.0f)? 255.0f : ((new_r < 0.0f)? 0.0f:new_r);
			new_g = (new_g > 255.0f)? 255.0f : ((new_g < 0.0f)? 0.0f:new_g);
			new_b = (new_b > 255.0f)? 255.0f : ((new_b < 0.0f)? 0.0f:new_b);

			new_pxl = gdImageColorAllocateAlpha(src, (int)new_r, (int)new_g, (int)new_b, new_a);
			if (new_pxl == -1) {
				new_pxl = gdImageColorClosestAlpha(src, (int)new_r, (int)new_g, (int)new_b, new_a);
			}
			if ((y >= 0) && (y < src->sy)) {
				gdImageSetPixel (src, x, y, new_pxl);
			}
		}
	}
	gdImageDestroy(srcback); 
	return 1; 
}