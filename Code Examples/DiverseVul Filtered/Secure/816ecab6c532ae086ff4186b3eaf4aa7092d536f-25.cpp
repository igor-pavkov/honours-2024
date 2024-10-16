static int PalettesAreEqual(Image *a,Image *b)
{
  ssize_t
    i;

  if ((a == (Image *) NULL) || (b == (Image *) NULL))
    return((int) MagickFalse);

  if (a->storage_class != PseudoClass || b->storage_class != PseudoClass)
    return((int) MagickFalse);

  if (a->colors != b->colors)
    return((int) MagickFalse);

  for (i=0; i < (ssize_t) a->colors; i++)
  {
    if ((a->colormap[i].red != b->colormap[i].red) ||
        (a->colormap[i].green != b->colormap[i].green) ||
        (a->colormap[i].blue != b->colormap[i].blue))
      return((int) MagickFalse);
  }

  return((int) MagickTrue);
}