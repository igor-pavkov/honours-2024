MagickExport MagickBooleanType DrawClipPath(Image *image,
  const DrawInfo *draw_info,const char *id,ExceptionInfo *exception)
{
  const char
    *clip_path;

  Image
    *clipping_mask;

  MagickBooleanType
    status;

  clip_path=GetImageArtifact(image,id);
  if (clip_path == (const char *) NULL)
    return(MagickFalse);
  clipping_mask=DrawClippingMask(image,draw_info,draw_info->clip_mask,clip_path,
    exception);
  if (clipping_mask == (Image *) NULL)
    return(MagickFalse);
  status=SetImageMask(image,WritePixelMask,clipping_mask,exception);
  clipping_mask=DestroyImage(clipping_mask);
  return(status);
}