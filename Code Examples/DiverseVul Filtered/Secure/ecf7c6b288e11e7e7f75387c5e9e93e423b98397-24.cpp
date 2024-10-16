static Image *DrawClippingMask(Image *image,const DrawInfo *draw_info,
  const char *id,const char *clip_path,ExceptionInfo *exception)
{
  DrawInfo
    *clone_info;

  Image
    *clip_mask,
    *separate_mask;

  MagickStatusType
    status;

  /*
    Draw a clip path.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(draw_info != (const DrawInfo *) NULL);
  clip_mask=AcquireImage((const ImageInfo *) NULL,exception);
  status=SetImageExtent(clip_mask,image->columns,image->rows,exception);
  if (status == MagickFalse)
    return(DestroyImage(clip_mask));
  status=SetImageMask(clip_mask,WritePixelMask,(Image *) NULL,exception);
  status=QueryColorCompliance("#0000",AllCompliance,
    &clip_mask->background_color,exception);
  clip_mask->background_color.alpha=(MagickRealType) TransparentAlpha;
  clip_mask->background_color.alpha_trait=BlendPixelTrait;
  status=SetImageBackgroundColor(clip_mask,exception);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(DrawEvent,GetMagickModule(),"\nbegin clip-path %s",
      id);
  clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  (void) CloneString(&clone_info->primitive,clip_path);
  status=QueryColorCompliance("#ffffff",AllCompliance,&clone_info->fill,
    exception);
  if (clone_info->clip_mask != (char *) NULL)
    clone_info->clip_mask=DestroyString(clone_info->clip_mask);
  status=QueryColorCompliance("#00000000",AllCompliance,&clone_info->stroke,
    exception);
  clone_info->stroke_width=0.0;
  clone_info->alpha=OpaqueAlpha;
  clone_info->clip_path=MagickTrue;
  status=RenderMVGContent(clip_mask,clone_info,0,exception);
  clone_info=DestroyDrawInfo(clone_info);
  separate_mask=SeparateImage(clip_mask,AlphaChannel,exception);
  if (separate_mask != (Image *) NULL)
    {
      clip_mask=DestroyImage(clip_mask);
      clip_mask=separate_mask;
      status=NegateImage(clip_mask,MagickFalse,exception);
      if (status == MagickFalse)
        clip_mask=DestroyImage(clip_mask);
    }
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(DrawEvent,GetMagickModule(),"end clip-path");
  return(clip_mask);
}