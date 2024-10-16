BGD_DECLARE(void) gdImageSetBrush (gdImagePtr im, gdImagePtr brush)
{
  int i;
  im->brush = brush;
  if ((!im->trueColor) && (!im->brush->trueColor))
    {
      for (i = 0; (i < gdImageColorsTotal (brush)); i++)
	{
	  int index;
	  index = gdImageColorResolveAlpha (im,
					    gdImageRed (brush, i),
					    gdImageGreen (brush, i),
					    gdImageBlue (brush, i),
					    gdImageAlpha (brush, i));
	  im->brushColorMap[i] = index;
	}
    }
}