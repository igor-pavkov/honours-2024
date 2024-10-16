BGD_DECLARE(void) gdImageCopyMergeGray (gdImagePtr dst, gdImagePtr src, int dstX, int dstY,
		      int srcX, int srcY, int w, int h, int pct)
{

  int c, dc;
  int x, y;
  int tox, toy;
  int ncR, ncG, ncB;
  float g;
  toy = dstY;
  for (y = srcY; (y < (srcY + h)); y++)
    {
      tox = dstX;
      for (x = srcX; (x < (srcX + w)); x++)
	{
	  int nc;
	  c = gdImageGetPixel (src, x, y);
	  /* Added 7/24/95: support transparent copies */
	  if (gdImageGetTransparent (src) == c)
	    {
	      tox++;
	      continue;
	    }
	  /* 
	   * If it's the same image, mapping is NOT trivial since we 
	   * merge with greyscale target, but if pct is 100, the grey 
	   * value is not used, so it becomes trivial. pjw 2.0.12. 
	   */
	  if (dst == src && pct == 100)
	    {
	      nc = c;
	    }
	  else
	    {
	      dc = gdImageGetPixel (dst, tox, toy);
	      g = 0.29900 * dst->red[dc]
		+ 0.58700 * dst->green[dc] + 0.11400 * dst->blue[dc];

	      ncR = gdImageRed (src, c) * (pct / 100.0)
		+ g * ((100 - pct) / 100.0);
	      ncG = gdImageGreen (src, c) * (pct / 100.0)
		+ g * ((100 - pct) / 100.0);
	      ncB = gdImageBlue (src, c) * (pct / 100.0)
		+ g * ((100 - pct) / 100.0);

	      /* First look for an exact match */
	      nc = gdImageColorExact (dst, ncR, ncG, ncB);
	      if (nc == (-1))
		{
		  /* No, so try to allocate it */
		  nc = gdImageColorAllocate (dst, ncR, ncG, ncB);
		  /* If we're out of colors, go for the
		     closest color */
		  if (nc == (-1))
		    {
		      nc = gdImageColorClosest (dst, ncR, ncG, ncB);
		    }
		}
	    }
	  gdImageSetPixel (dst, tox, toy, nc);
	  tox++;
	}
      toy++;
    }
}