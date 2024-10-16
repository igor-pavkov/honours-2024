BGD_DECLARE(void) gdImageCopyRotated (gdImagePtr dst,
		    gdImagePtr src,
		    double dstX, double dstY,
		    int srcX, int srcY,
		    int srcWidth, int srcHeight, int angle)
{
  double dx, dy;
  double radius = sqrt (srcWidth * srcWidth + srcHeight * srcHeight);
  double aCos = cos (angle * .0174532925);
  double aSin = sin (angle * .0174532925);
  double scX = srcX + ((double) srcWidth) / 2;
  double scY = srcY + ((double) srcHeight) / 2;
  int cmap[gdMaxColors];
  int i;
  for (i = 0; (i < gdMaxColors); i++)
    {
      cmap[i] = (-1);
    }
  for (dy = dstY - radius; (dy <= dstY + radius); dy++)
    {
      for (dx = dstX - radius; (dx <= dstX + radius); dx++)
	{
	  double sxd = (dx - dstX) * aCos - (dy - dstY) * aSin;
	  double syd = (dy - dstY) * aCos + (dx - dstX) * aSin;
	  int sx = sxd + scX;
	  int sy = syd + scY;
	  if ((sx >= srcX) && (sx < srcX + srcWidth) &&
	      (sy >= srcY) && (sy < srcY + srcHeight))
	    {
	      int c = gdImageGetPixel (src, sx, sy);
	      if (!src->trueColor)
		{
		  /* Use a table to avoid an expensive
		     lookup on every single pixel */
		  if (cmap[c] == -1)
		    {
		      cmap[c] = gdImageColorResolveAlpha (dst,
							  gdImageRed (src, c),
							  gdImageGreen (src,
									c),
							  gdImageBlue (src,
								       c),
							  gdImageAlpha (src,
									c));
		    }
		  gdImageSetPixel (dst, dx, dy, cmap[c]);
		}
	      else
		{
		  gdImageSetPixel (dst,
				   dx, dy,
				   gdImageColorResolveAlpha (dst,
							     gdImageRed (src,
									 c),
							     gdImageGreen
							     (src, c),
							     gdImageBlue (src,
									  c),
							     gdImageAlpha
							     (src, c)));
		}
	    }
	}
    }
}