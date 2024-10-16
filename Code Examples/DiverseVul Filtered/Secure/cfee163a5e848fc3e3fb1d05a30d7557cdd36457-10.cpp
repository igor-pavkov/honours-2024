BGD_DECLARE(void) gdImagePolygon (gdImagePtr im, gdPointPtr p, int n, int c)
{
  if (!n)
    {
      return;
    }


  gdImageLine (im, p->x, p->y, p[n - 1].x, p[n - 1].y, c);
  gdImageOpenPolygon (im, p, n, c);
}