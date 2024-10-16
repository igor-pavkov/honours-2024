void gdImageSkewX (gdImagePtr dst, gdImagePtr src, int uRow, int iOffset, double dWeight, int clrBack)
{
	typedef int (*FuncPtr)(gdImagePtr, int, int);
	int i, r, g, b, a;
	FuncPtr f;

	int pxlOldLeft, pxlLeft=0, pxlSrc;

	if (src->trueColor) {
		f = gdImageGetTrueColorPixel;
	} else {
		f = gdImageGetPixel;
	}

	for (i = 0; i < iOffset; i++) {
		gdImageSetPixel (dst, i, uRow, clrBack);
	}

	if (i < dst->sx) {
		gdImageSetPixel (dst, i, uRow, clrBack);
	}

	pxlOldLeft  = clrBack;

	for (i = 0; i < src->sx; i++) {
		pxlSrc = f (src,i,uRow);

		r = (int)(gdImageRed(src,pxlSrc) * dWeight);
		g = (int)(gdImageGreen(src,pxlSrc) * dWeight);
		b = (int)(gdImageBlue(src,pxlSrc) * dWeight);
		a = (int)(gdImageAlpha(src,pxlSrc) * dWeight);

		pxlLeft = gdImageColorAllocateAlpha(src, r, g, b, a);
		
		if (pxlLeft == -1) {
			pxlLeft = gdImageColorClosestAlpha(src, r, g, b, a);
		}
		
		r = gdImageRed(src,pxlSrc) - (gdImageRed(src,pxlLeft) - gdImageRed(src,pxlOldLeft));
		g = gdImageGreen(src,pxlSrc) - (gdImageGreen(src,pxlLeft) - gdImageGreen(src,pxlOldLeft));
		b = gdImageBlue(src,pxlSrc) - (gdImageBlue(src,pxlLeft) - gdImageBlue(src,pxlOldLeft));
		a = gdImageAlpha(src,pxlSrc) - (gdImageAlpha(src,pxlLeft) - gdImageAlpha(src,pxlOldLeft));

	        if (r>255) {
        		r = 255;
        	}
        	
		if (g>255) {
			g = 255;
		}	
	
	        if(b>255) {
	        	b = 255;
	        }
	        
		if (a>127) {
			b = 127;
		}

		pxlSrc = gdImageColorAllocateAlpha(dst, r, g, b, a);

	        if (pxlSrc == -1) {
			pxlSrc = gdImageColorClosestAlpha(dst, r, g, b, a);
		}

		if ((i + iOffset >= 0) && (i + iOffset < dst->sx)) {
			gdImageSetPixel (dst, i+iOffset, uRow,  pxlSrc);
		}

		pxlOldLeft = pxlLeft;
	}

	i += iOffset;
	
	if (i < dst->sx) {
		gdImageSetPixel (dst, i, uRow, pxlLeft);
	}

	gdImageSetPixel (dst, iOffset, uRow, clrBack);

	i--;

	while (++i < dst->sx) {
		gdImageSetPixel (dst, i, uRow, clrBack);
	}
}