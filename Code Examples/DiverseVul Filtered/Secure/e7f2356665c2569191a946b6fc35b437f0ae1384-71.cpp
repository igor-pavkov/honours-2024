inline static void gdImageSetAAPixelColor(gdImagePtr im, int x, int y, int color, int t)
{
	int dr,dg,db,p,r,g,b;
	dr = gdTrueColorGetRed(color);
	dg = gdTrueColorGetGreen(color);
	db = gdTrueColorGetBlue(color);

	p = gdImageGetPixel(im,x,y);
	r = gdTrueColorGetRed(p);
	g = gdTrueColorGetGreen(p);
	b = gdTrueColorGetBlue(p);

	BLEND_COLOR(t, dr, r, dr);
	BLEND_COLOR(t, dg, g, dg);
	BLEND_COLOR(t, db, b, db);
	im->tpixels[y][x]=gdTrueColorAlpha(dr, dg, db,  gdAlphaOpaque);
}