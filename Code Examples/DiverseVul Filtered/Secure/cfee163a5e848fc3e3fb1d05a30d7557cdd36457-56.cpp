static void gdImageSetAAPixelColor(gdImagePtr im, int x, int y, int color, int t)
{
	int dr,dg,db,p,r,g,b;
	p = gdImageGetPixel(im,x,y);
        /* TBB: we have to implement the dont_blend stuff to provide
          the full feature set of the old implementation */
        if ((p == color)
	  || ((p == im->AA_dont_blend)
	      && (t != 0x00)))
        {
          return;
        }
	dr = gdTrueColorGetRed(color);
	dg = gdTrueColorGetGreen(color);
	db = gdTrueColorGetBlue(color);

	r = gdTrueColorGetRed(p);
	g = gdTrueColorGetGreen(p);
	b = gdTrueColorGetBlue(p);

	BLEND_COLOR(t, dr, r, dr);
	BLEND_COLOR(t, dg, g, dg);
	BLEND_COLOR(t, db, b, db);
	im->tpixels[y][x]=gdTrueColorAlpha(dr, dg, db,  gdAlphaOpaque);
}  