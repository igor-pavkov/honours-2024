BGD_DECLARE(void) gdImageStringUp (gdImagePtr im, gdFontPtr f,
		 int x, int y, unsigned char *s, int color)
{
  int i;
  int l;
  l = strlen ((char *) s);
  for (i = 0; (i < l); i++)
    {
      gdImageCharUp (im, f, x, y, s[i], color);
      y -= f->w;
    }
}