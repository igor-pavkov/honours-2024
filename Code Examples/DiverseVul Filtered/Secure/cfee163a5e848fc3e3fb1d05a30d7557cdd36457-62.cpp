BGD_DECLARE(int) gdImageCompare (gdImagePtr im1, gdImagePtr im2)
{
  int x, y;
  int p1, p2;
  int cmpStatus = 0;
  int sx, sy;

  if (im1->interlace != im2->interlace)
    {
      cmpStatus |= GD_CMP_INTERLACE;
    }

  if (im1->transparent != im2->transparent)
    {
      cmpStatus |= GD_CMP_TRANSPARENT;
    }

  if (im1->trueColor != im2->trueColor)
    {
      cmpStatus |= GD_CMP_TRUECOLOR;
    }

  sx = im1->sx;
  if (im1->sx != im2->sx)
    {
      cmpStatus |= GD_CMP_SIZE_X + GD_CMP_IMAGE;
      if (im2->sx < im1->sx)
	{
	  sx = im2->sx;
	}
    }

  sy = im1->sy;
  if (im1->sy != im2->sy)
    {
      cmpStatus |= GD_CMP_SIZE_Y + GD_CMP_IMAGE;
      if (im2->sy < im1->sy)
	{
	  sy = im2->sy;
	}
    }

  if (im1->colorsTotal != im2->colorsTotal)
    {
      cmpStatus |= GD_CMP_NUM_COLORS;
    }

  for (y = 0; (y < sy); y++)
    {
      for (x = 0; (x < sx); x++)
	{
	  p1 =
	    im1->trueColor ? gdImageTrueColorPixel (im1, x,
						    y) :
	    gdImagePalettePixel (im1, x, y);
	  p2 =
	    im2->trueColor ? gdImageTrueColorPixel (im2, x,
						    y) :
	    gdImagePalettePixel (im2, x, y);
	  if (gdImageRed (im1, p1) != gdImageRed (im2, p2))
	    {
	      cmpStatus |= GD_CMP_COLOR + GD_CMP_IMAGE;
	      break;
	    }
	  if (gdImageGreen (im1, p1) != gdImageGreen (im2, p2))
	    {
	      cmpStatus |= GD_CMP_COLOR + GD_CMP_IMAGE;
	      break;
	    }
	  if (gdImageBlue (im1, p1) != gdImageBlue (im2, p2))
	    {
	      cmpStatus |= GD_CMP_COLOR + GD_CMP_IMAGE;
	      break;
	    }
#if 0
	  /* Soon we'll add alpha channel to palettes */
	  if (gdImageAlpha (im1, p1) != gdImageAlpha (im2, p2))
	    {
	      cmpStatus |= GD_CMP_COLOR + GD_CMP_IMAGE;
	      break;
	    }
#endif
	}
      if (cmpStatus & GD_CMP_COLOR)
	{
	  break;
	};
    }

  return cmpStatus;
}