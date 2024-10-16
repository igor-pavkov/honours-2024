gdImagePtr gdImageRotate (gdImagePtr src, double dAngle, int clrBack)
{
	gdImagePtr pMidImg;
	gdImagePtr rotatedImg;

	if (src == NULL) {
		return NULL;
	}

	while (dAngle >= 360.0) {
		dAngle -= 360.0;
	}

	while (dAngle < 0) {
		dAngle += 360.0;
	}
    
	if (dAngle == 90.00) {
		return gdImageRotate90(src);
	}
	if (dAngle == 180.00) {
		return gdImageRotate180(src);
	}
	if(dAngle == 270.00) {
		return gdImageRotate270 ( src);
	}

	if ((dAngle > 45.0) && (dAngle <= 135.0)) {
		pMidImg = gdImageRotate90 (src);
		dAngle -= 90.0;
	} else if ((dAngle > 135.0) && (dAngle <= 225.0)) {
		pMidImg = gdImageRotate180 (src);
		dAngle -= 180.0;
	} else if ((dAngle > 225.0) && (dAngle <= 315.0)) {
		pMidImg = gdImageRotate270 (src);
		dAngle -= 270.0;
	} else {
		return gdImageRotate45 (src, dAngle, clrBack);
	}
    
	if (pMidImg == NULL) {
		return NULL;
	}
	
	rotatedImg = gdImageRotate45 (pMidImg, dAngle, clrBack);
	gdImageDestroy(pMidImg);
	
	return rotatedImg;
}