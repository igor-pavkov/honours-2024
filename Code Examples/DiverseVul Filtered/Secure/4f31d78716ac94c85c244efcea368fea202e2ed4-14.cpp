static void ClearBounds(Image *image,RectangleInfo *bounds)
{
  ExceptionInfo
    *exception;

  ssize_t
    y;

  if (bounds->x < 0)
    return;
  if (image->matte == MagickFalse)
    (void) SetImageAlphaChannel(image,OpaqueAlphaChannel);
  exception=(&image->exception);
  for (y=0; y < (ssize_t) bounds->height; y++)
  {
    register ssize_t
      x;

    register PixelPacket
      *magick_restrict q;

    q=GetAuthenticPixels(image,bounds->x,bounds->y+y,bounds->width,1,exception);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (ssize_t) bounds->width; x++)
    {
      q->opacity=(Quantum) TransparentOpacity;
      q++;
    }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      break;
  }
}