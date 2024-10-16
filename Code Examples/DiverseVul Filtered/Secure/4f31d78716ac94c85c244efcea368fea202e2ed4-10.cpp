static MagickBooleanType IsBoundsCleared(const Image *image1,
  const Image *image2,RectangleInfo *bounds,ExceptionInfo *exception)
{
  register const PixelPacket
    *p,
    *q;

  register ssize_t
    x;

  ssize_t
    y;

  if (bounds->x < 0)
    return(MagickFalse);
  for (y=0; y < (ssize_t) bounds->height; y++)
  {
    p=GetVirtualPixels(image1,bounds->x,bounds->y+y,bounds->width,1,
      exception);
    q=GetVirtualPixels(image2,bounds->x,bounds->y+y,bounds->width,1,
      exception);
    if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      break;
    for (x=0; x < (ssize_t) bounds->width; x++)
    {
      if ((GetPixelOpacity(p) <= (Quantum) (QuantumRange/2)) &&
          (GetPixelOpacity(q) > (Quantum) (QuantumRange/2)))
        break;
      p++;
      q++;
    }
    if (x < (ssize_t) bounds->width)
      break;
  }
  return(y < (ssize_t) bounds->height ? MagickTrue : MagickFalse);
}