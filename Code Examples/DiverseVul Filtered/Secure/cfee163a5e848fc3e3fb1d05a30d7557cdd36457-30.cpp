BGD_DECLARE(void) gdImageCharUp (gdImagePtr im, gdFontPtr f, int x, int y, int c, int color)
{
  int cx, cy;
  int px, py;
  int fline;
  cx = 0;
  cy = 0;
#ifdef CHARSET_EBCDIC
  c = ASC (c);
#endif /*CHARSET_EBCDIC */
  if ((c < f->offset) || (c >= (f->offset + f->nchars)))
    {
      return;
    }
  fline = (c - f->offset) * f->h * f->w;
  for (py = y; (py > (y - f->w)); py--)
    {
      for (px = x; (px < (x + f->h)); px++)
	{
	  if (f->data[fline + cy * f->w + cx])
	    {
	      gdImageSetPixel (im, px, py, color);
	    }
	  cy++;
	}
      cy = 0;
      cx++;
    }
}