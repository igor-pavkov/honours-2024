static int gdLayerOverlay (int dst, int src)
{
	int a1, a2;
	a1 = gdAlphaMax - gdTrueColorGetAlpha(dst);
	a2 = gdAlphaMax - gdTrueColorGetAlpha(src);
	return ( ((gdAlphaMax - a1*a2/gdAlphaMax) << 24) +
		(gdAlphaOverlayColor( gdTrueColorGetRed(src), gdTrueColorGetRed(dst), gdRedMax ) << 16) +
		(gdAlphaOverlayColor( gdTrueColorGetGreen(src), gdTrueColorGetGreen(dst), gdGreenMax ) << 8) +
		(gdAlphaOverlayColor( gdTrueColorGetBlue(src), gdTrueColorGetBlue(dst), gdBlueMax ))
		);
}