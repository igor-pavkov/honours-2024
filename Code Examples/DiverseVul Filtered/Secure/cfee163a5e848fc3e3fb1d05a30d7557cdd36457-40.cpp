BGD_DECLARE(void) gdImageSetAntiAliased (gdImagePtr im, int c)
{
  im->AA = 1;
  im->AA_color = c;
  im->AA_dont_blend = -1;
}