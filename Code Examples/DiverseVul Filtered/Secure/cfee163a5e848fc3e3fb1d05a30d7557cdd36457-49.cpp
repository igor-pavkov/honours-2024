BGD_DECLARE(void) gdImageSetPixel (gdImagePtr im, int x, int y, int color)
{
  int p;
  switch (color)
    {
    case gdStyled:
      if (!im->style)
	{
	  /* Refuse to draw if no style is set. */
	  return;
	}
      else
	{
	  p = im->style[im->stylePos++];
	}
      if (p != (gdTransparent))
	{
	  gdImageSetPixel (im, x, y, p);
	}
      im->stylePos = im->stylePos % im->styleLength;
      break;
    case gdStyledBrushed:
      if (!im->style)
	{
	  /* Refuse to draw if no style is set. */
	  return;
	}
      p = im->style[im->stylePos++];
      if ((p != gdTransparent) && (p != 0))
	{
	  gdImageSetPixel (im, x, y, gdBrushed);
	}
      im->stylePos = im->stylePos % im->styleLength;
      break;
    case gdBrushed:
      gdImageBrushApply (im, x, y);
      break;
    case gdTiled:
      gdImageTileApply (im, x, y);
      break;
    case gdAntiAliased:
      /* This shouldn't happen (2.0.26) because we just call
        gdImageAALine now, but do something sane. */
      gdImageSetPixel(im, x, y, im->AA_color);
      break;
    default:
      if (gdImageBoundsSafeMacro (im, x, y))
	{
	  if (im->trueColor)
	    {
	      if (im->alphaBlendingFlag)
		{
		  im->tpixels[y][x] = gdAlphaBlend (im->tpixels[y][x], color);
		}
	      else
		{
		  im->tpixels[y][x] = color;
		}
	    }
	  else
	    {
	      im->pixels[y][x] = color;
	    }
	}
      break;
    }
}