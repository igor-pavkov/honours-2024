BGD_DECLARE(void) gdImageOpenPolygon (gdImagePtr im, gdPointPtr p, int n, int c)
{
  int i;
  int lx, ly;
  if (!n)
    {
      return;
    }


  lx = p->x;
  ly = p->y;
  for (i = 1; (i < n); i++)
    {
      p++;
      gdImageLine (im, lx, ly, p->x, p->y, c);
      lx = p->x;
      ly = p->y;
    }

}