int gdImageSelectiveBlur( gdImagePtr src)
{
	int         x, y, i, j;
	float       new_r, new_g, new_b;
	int         new_pxl, cpxl, pxl, new_a=0;
	float flt_r [3][3];
	float flt_g [3][3];
	float flt_b [3][3];
	float flt_r_sum, flt_g_sum, flt_b_sum;

	gdImagePtr srcback;
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

	for(y = 0; y<src->sy; y++) {
		for (x=0; x<src->sx; x++) {
		      flt_r_sum = flt_g_sum = flt_b_sum = 0.0;
			cpxl = f(src, x, y); 

			for (j=0; j<3; j++) {
				for (i=0; i<3; i++) {
					if ((j == 1) && (i == 1)) {
						flt_r[1][1] = flt_g[1][1] = flt_b[1][1] = 0.5;
					} else {
						pxl = f(src, x-(3>>1)+i, y-(3>>1)+j); 
						new_a = gdImageAlpha(srcback, pxl);

						new_r = ((float)gdImageRed(srcback, cpxl)) - ((float)gdImageRed (srcback, pxl));
						
						if (new_r < 0.0f) {
							new_r = -new_r;
						}	
						if (new_r != 0) {
							flt_r[j][i] = 1.0f/new_r;
						} else {
							flt_r[j][i] = 1.0f;
						}	

						new_g = ((float)gdImageGreen(srcback, cpxl)) - ((float)gdImageGreen(srcback, pxl));
						
						if (new_g < 0.0f) {
							new_g = -new_g;
						}	
						if (new_g != 0) {
							flt_g[j][i] = 1.0f/new_g;
						} else {
							flt_g[j][i] = 1.0f;
						}	

						new_b = ((float)gdImageBlue(srcback, cpxl)) - ((float)gdImageBlue(srcback, pxl));

						if (new_b < 0.0f) {
							new_b = -new_b;
						}	
						if (new_b != 0) {
							flt_b[j][i] = 1.0f/new_b;
						} else {
							flt_b[j][i] = 1.0f;
						}
					}
						
					flt_r_sum += flt_r[j][i];
					flt_g_sum += flt_g[j][i];
					flt_b_sum += flt_b [j][i];
				}
			}
      
			for (j=0; j<3; j++) {
				for (i=0; i<3; i++) {
					if (flt_r_sum != 0.0) {
						flt_r[j][i] /= flt_r_sum;
					}	
					if (flt_g_sum != 0.0) {
						flt_g[j][i] /= flt_g_sum;
					}	
					if (flt_b_sum != 0.0) {
						flt_b [j][i] /= flt_b_sum;
					}	
				}
			}

			new_r = new_g = new_b = 0.0;
			
			for (j=0; j<3; j++) {
				for (i=0; i<3; i++) {
					pxl = f(src, x-(3>>1)+i, y-(3>>1)+j);
					new_r += (float)gdImageRed(srcback, pxl) * flt_r[j][i];
					new_g += (float)gdImageGreen(srcback, pxl) * flt_g[j][i];
					new_b += (float)gdImageBlue(srcback, pxl) * flt_b[j][i];
				}
			}

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