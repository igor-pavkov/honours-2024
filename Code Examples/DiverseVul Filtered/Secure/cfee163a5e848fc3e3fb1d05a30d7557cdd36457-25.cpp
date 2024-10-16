BGD_DECLARE(void) gdImageSetAntiAliasedDontBlend (gdImagePtr im, int c, int dont_blend)
{
  im->AA = 1;
  im->AA_color = c;
  im->AA_dont_blend = dont_blend;
}