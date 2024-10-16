BGD_DECLARE(void) gdImageLine (gdImagePtr im, int x1, int y1, int x2, int y2, int color)
{
  int dx, dy, incr1, incr2, d, x, y, xend, yend, xdirflag, ydirflag;
  int wid;
  int w, wstart;
  int thick;
  if (color == gdAntiAliased)
    {
      /* 
        gdAntiAliased passed as color: use the much faster, much cheaper
        and equally attractive gdImageAALine implementation. That
        clips too, so don't clip twice.
      */
      gdImageAALine(im, x1, y1, x2, y2, im->AA_color); 
      return;
    }
  /* 2.0.10: Nick Atty: clip to edges of drawing rectangle, return if no
     points need to be drawn. 2.0.26, TBB: clip to edges of clipping
     rectangle. We were getting away with this because gdImageSetPixel
     is used for actual drawing, but this is still more efficient and opens
     the way to skip per-pixel bounds checking in the future. */

  if (clip_1d (&x1, &y1, &x2, &y2, im->cx1, im->cx2) == 0)
    return;
  if (clip_1d (&y1, &x1, &y2, &x2, im->cy1, im->cy2) == 0)
    return;
  thick = im->thick;

  dx = abs (x2 - x1);
  dy = abs (y2 - y1);
  if (dy <= dx)
    {
      /* More-or-less horizontal. use wid for vertical stroke */
      /* Doug Claar: watch out for NaN in atan2 (2.0.5) */
      if ((dx == 0) && (dy == 0))
	{
	  wid = 1;
	}
      else
	{
	  /* 2.0.12: Michael Schwartz: divide rather than multiply;
	     TBB: but watch out for /0! */
	  double ac = cos (atan2 (dy, dx));
	  if (ac != 0)
	    {
	      wid = thick / ac;
	    }
	  else
	    {
	      wid = 1;
	    }
	  if (wid == 0)
	    {
	      wid = 1;
	    }
	}
      d = 2 * dy - dx;
      incr1 = 2 * dy;
      incr2 = 2 * (dy - dx);
      if (x1 > x2)
	{
	  x = x2;
	  y = y2;
	  ydirflag = (-1);
	  xend = x1;
	}
      else
	{
	  x = x1;
	  y = y1;
	  ydirflag = 1;
	  xend = x2;
	}

      /* Set up line thickness */
      wstart = y - wid / 2;
      for (w = wstart; w < wstart + wid; w++)
	gdImageSetPixel (im, x, w, color);

      if (((y2 - y1) * ydirflag) > 0)
	{
	  while (x < xend)
	    {
	      x++;
	      if (d < 0)
		{
		  d += incr1;
		}
	      else
		{
		  y++;
		  d += incr2;
		}
	      wstart = y - wid / 2;
	      for (w = wstart; w < wstart + wid; w++)
		gdImageSetPixel (im, x, w, color);
	    }
	}
      else
	{
	  while (x < xend)
	    {
	      x++;
	      if (d < 0)
		{
		  d += incr1;
		}
	      else
		{
		  y--;
		  d += incr2;
		}
	      wstart = y - wid / 2;
	      for (w = wstart; w < wstart + wid; w++)
		gdImageSetPixel (im, x, w, color);
	    }
	}
    }
  else
    {
      /* More-or-less vertical. use wid for horizontal stroke */
      /* 2.0.12: Michael Schwartz: divide rather than multiply;
         TBB: but watch out for /0! */
      double as = sin (atan2 (dy, dx));
      if (as != 0)
	{
	  wid = thick / as;
	}
      else
	{
	  wid = 1;
	}
      if (wid == 0)
	wid = 1;

      d = 2 * dx - dy;
      incr1 = 2 * dx;
      incr2 = 2 * (dx - dy);
      if (y1 > y2)
	{
	  y = y2;
	  x = x2;
	  yend = y1;
	  xdirflag = (-1);
	}
      else
	{
	  y = y1;
	  x = x1;
	  yend = y2;
	  xdirflag = 1;
	}

      /* Set up line thickness */
      wstart = x - wid / 2;
      for (w = wstart; w < wstart + wid; w++)
	gdImageSetPixel (im, w, y, color);

      if (((x2 - x1) * xdirflag) > 0)
	{
	  while (y < yend)
	    {
	      y++;
	      if (d < 0)
		{
		  d += incr1;
		}
	      else
		{
		  x++;
		  d += incr2;
		}
	      wstart = x - wid / 2;
	      for (w = wstart; w < wstart + wid; w++)
		gdImageSetPixel (im, w, y, color);
	    }
	}
      else
	{
	  while (y < yend)
	    {
	      y++;
	      if (d < 0)
		{
		  d += incr1;
		}
	      else
		{
		  x--;
		  d += incr2;
		}
	      wstart = x - wid / 2;
	      for (w = wstart; w < wstart + wid; w++)
		gdImageSetPixel (im, w, y, color);
	    }
	}
    }

}