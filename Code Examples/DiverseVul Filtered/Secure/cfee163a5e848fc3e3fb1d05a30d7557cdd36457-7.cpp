BGD_DECLARE(void) gdImageCopy (gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX,
	     int srcY, int w, int h)
{
  int c;
  int x, y;
  int tox, toy;
  int i;
  int colorMap[gdMaxColors];

  if (dst->trueColor) {
	  /* 2.0: much easier when the destination is truecolor. */
	  /* 2.0.10: needs a transparent-index check that is still valid if
	   *          * the source is not truecolor. Thanks to Frank Warmerdam.
	   *                   */

	  if (src->trueColor) {
		  for (y = 0; (y < h); y++) {
			  for (x = 0; (x < w); x++) {
				  int c = gdImageGetTrueColorPixel (src, srcX + x, srcY + y);
				  gdImageSetPixel (dst, dstX + x, dstY + y, c);
			  }
		  }
	  } else {
		  /* source is palette based */
		  for (y = 0; (y < h); y++) {
			  for (x = 0; (x < w); x++) {
				  int c = gdImageGetPixel (src, srcX + x, srcY + y);
				  if (c != src->transparent) {
					  gdImageSetPixel(dst, dstX + x, dstY + y, gdTrueColorAlpha(src->red[c], src->green[c], src->blue[c], src->alpha[c]));
				  }
			  }
		  }
	  }
	  return;
  }

  for (i = 0; (i < gdMaxColors); i++)
  {
	  colorMap[i] = (-1);
  }
  toy = dstY;
  for (y = srcY; (y < (srcY + h)); y++)
  {
	  tox = dstX;
	  for (x = srcX; (x < (srcX + w)); x++)
	  {
		  int nc;
		  int mapTo;
		  c = gdImageGetPixel (src, x, y);
		  /* Added 7/24/95: support transparent copies */
		  if (gdImageGetTransparent (src) == c)
		  {
			  tox++;
			  continue;
		  }
		  /* Have we established a mapping for this color? */
		  if (src->trueColor)
		  {
			  /* 2.05: remap to the palette available in the
				 destination image. This is slow and
				 works badly, but it beats crashing! Thanks 
				 to Padhrig McCarthy. */
	      mapTo = gdImageColorResolveAlpha (dst,
						gdTrueColorGetRed (c),
						gdTrueColorGetGreen (c),
						gdTrueColorGetBlue (c),
						gdTrueColorGetAlpha (c));
	    }
	  else if (colorMap[c] == (-1))
	    {
	      /* If it's the same image, mapping is trivial */
	      if (dst == src)
		{
		  nc = c;
		}
	      else
		{
		  /* Get best match possible. This
		     function never returns error. */
		  nc = gdImageColorResolveAlpha (dst,
						 src->red[c], src->green[c],
						 src->blue[c], src->alpha[c]);
		}
	      colorMap[c] = nc;
	      mapTo = colorMap[c];
	    }
	  else
	    {
	      mapTo = colorMap[c];
	    }
	  gdImageSetPixel (dst, tox, toy, mapTo);
	  tox++;
	}
      toy++;
    }
}