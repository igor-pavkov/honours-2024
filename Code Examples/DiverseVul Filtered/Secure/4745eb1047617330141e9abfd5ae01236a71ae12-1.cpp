static MagickBooleanType ApplyPSDLayerOpacity(Image *image,Quantum opacity,
  MagickBooleanType revert,ExceptionInfo *exception)
{
  MagickBooleanType
    status;

  ssize_t
    y;

  if (image->debug != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "  applying layer opacity %.20g", (double) opacity);
  if (opacity == QuantumRange)
    return(MagickTrue);
  if (image->matte != MagickTrue)
    (void) SetImageAlphaChannel(image,OpaqueAlphaChannel);
  status=MagickTrue;
#if defined(MAGICKCORE_OPENMP_SUPPORT)
#pragma omp parallel for schedule(static) shared(status) \
  magick_number_threads(image,image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register PixelPacket
      *magick_restrict q;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    q=GetAuthenticPixels(image,0,y,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      if (revert == MagickFalse)
        SetPixelAlpha(q,(Quantum) (QuantumScale*(GetPixelAlpha(q)*opacity)));
      else if (opacity > 0)
        SetPixelAlpha(q,(Quantum) (QuantumRange*(GetPixelAlpha(q)/
          (MagickRealType) opacity)));
      q++;
    }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      status=MagickFalse;
  }
  return(status);
}