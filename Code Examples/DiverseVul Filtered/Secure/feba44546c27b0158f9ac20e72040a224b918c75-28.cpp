gdImagePtr gdImageRotate45 (gdImagePtr src, double dAngle, int clrBack)
{
	typedef int (*FuncPtr)(gdImagePtr, int, int);
	gdImagePtr dst1,dst2,dst3;
	FuncPtr f;
	double dRadAngle, dSinE, dTan, dShear;
	double dOffset;     /* Variable skew offset */
	int u, iShear, newx, newy;

	/* See GEMS I for the algorithm details */
	dRadAngle = dAngle * ROTATE_DEG2RAD; /* Angle in radians */
	dSinE = sin (dRadAngle);
	dTan = tan (dRadAngle / 2.0);

	newx = (int)(src->sx + src->sy * fabs(dTan));
	newy = src->sy;

	/* 1st shear */
	if (src->trueColor) {
		dst1 = gdImageCreateTrueColor (newx, newy);
		f = gdImageGetTrueColorPixel;
	} else {
		dst1 = gdImageCreate (newx, newy);
		f = gdImageGetPixel;
	}

	/******* Perform 1st shear (horizontal) ******/
	if (dst1 == NULL) {
		return NULL;
	}

	if (dAngle == 0.0) {
		/* Returns copy of src */
		gdImageCopy (dst1, src,0,0,0,0,src->sx,src->sy);
		return dst1;
	}
	
	gdImagePaletteCopy (dst1, src);
	dRadAngle = dAngle * ROTATE_DEG2RAD; /* Angle in radians */
	dSinE = sin (dRadAngle);
	dTan = tan (dRadAngle / 2.0);

	for (u = 0; u < dst1->sy; u++) {
		if (dTan >= 0.0) {
			dShear = ((double)(u + 0.5)) * dTan;
		} else {
			dShear = ((double)(u - dst1->sy) + 0.5) * dTan;
		}

		iShear = (int)floor(dShear);
		
		gdImageSkewX(dst1, src, u, iShear, (dShear - iShear), clrBack);
	}

	/* 2nd shear */
	newx = dst1->sx;
	
	if (dSinE > 0.0) {
		dOffset = (src->sx ) * dSinE;
	} else {
		dOffset = -dSinE *  (src->sx - newx);
	}

	newy = (int) ((double) src->sx * fabs( dSinE ) + (double) src->sy * cos (dRadAngle));

	if (src->trueColor) {
		dst2 = gdImageCreateTrueColor (newx, newy);
		f = gdImageGetTrueColorPixel;
	} else {
		dst2 = gdImageCreate (newx, newy);
		f = gdImageGetPixel;
	}

	if (dst2 == NULL) {
		gdImageDestroy(dst1);
		return NULL;
	}
	
	for (u = 0; u < dst2->sx; u++, dOffset -= dSinE) {
		iShear = (int)floor (dOffset);
		gdImageSkewY(dst2, dst1, u, iShear, (dOffset - iShear), clrBack);
	}


	/* 3rd shear */
	gdImageDestroy(dst1);

	newx = (int) ((double)src->sy * fabs (dSinE) + (double)src->sx * cos (dRadAngle)) + 1;
	newy = dst2->sy;
    
	if (src->trueColor) {
		dst3 = gdImageCreateTrueColor (newx, newy);
		f = gdImageGetTrueColorPixel;
	} else {
		dst3 = gdImageCreate (newx, newy);
		f = gdImageGetPixel;
	}
	
	if (dst3 == NULL) {
		gdImageDestroy(dst2);
		return NULL;
	}
	if (dSinE >= 0.0) {
		dOffset = (double)(src->sx - 1) * dSinE * -dTan;
	} else {
		dOffset = dTan * ((double)(src->sx - 1) * -dSinE + (double)(1 - newy));
	}

	for (u = 0; u < dst3->sy; u++, dOffset += dTan) {
	        int iShear = (int)floor(dOffset);
		gdImageSkewX(dst3, dst2, u, iShear, (dOffset - iShear), clrBack);
	}

	gdImageDestroy(dst2);
	
	return dst3;
}