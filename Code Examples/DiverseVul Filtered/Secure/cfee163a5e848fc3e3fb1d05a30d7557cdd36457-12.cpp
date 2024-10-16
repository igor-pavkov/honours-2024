BGD_DECLARE(void) gdImageStringUp16 (gdImagePtr im, gdFontPtr f,
		   int x, int y, unsigned short *s, int color)
{
  int i;
  int l;
  l = strlen16 (s);
  for (i = 0; (i < l); i++)
    {
      gdImageCharUp (im, f, x, y, s[i], color);
      y -= f->w;
    }
}