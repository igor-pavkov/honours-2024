BGD_DECLARE(void) gdImageSetTile (gdImagePtr im, gdImagePtr tile)
{
  int i;
  im->tile = tile;
  if ((!im->trueColor) && (!im->tile->trueColor))
    {
      for (i = 0; (i < gdImageColorsTotal (tile)); i++)
	{
	  int index;
	  index = gdImageColorResolveAlpha (im,
					    gdImageRed (tile, i),
					    gdImageGreen (tile, i),
					    gdImageBlue (tile, i),
					    gdImageAlpha (tile, i));
	  im->tileColorMap[i] = index;
	}
    }
}