static Image *DrawCompositeMask(Image *image,const DrawInfo *draw_info,
  const char *id,const char *mask_path,ExceptionInfo *exception)
{
  Image
    *composite_mask,
    *separate_mask;

  DrawInfo
    *clone_info;

  MagickStatusType
    status;

  /*
    Draw a mask path.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(draw_info != (const DrawInfo *) NULL);
  composite_mask=AcquireImage((const ImageInfo *) NULL,exception);
  status=SetImageExtent(composite_mask,image->columns,image->rows,exception);
  if (status == MagickFalse)
    return(DestroyImage(composite_mask));
  status=SetImageMask(composite_mask,CompositePixelMask,(Image *) NULL,
    exception);
  status=QueryColorCompliance("#0000",AllCompliance,
    &composite_mask->background_color,exception);
  composite_mask->background_color.alpha=(MagickRealType) TransparentAlpha;
  composite_mask->background_color.alpha_trait=BlendPixelTrait;
  (void) SetImageBackgroundColor(composite_mask,exception);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(DrawEvent,GetMagickModule(),"\nbegin mask-path %s",
      id);
  clone_info=CloneDrawInfo((ImageInfo *) NULL,draw_info);
  (void) CloneString(&clone_info->primitive,mask_path);
  status=QueryColorCompliance("#ffffff",AllCompliance,&clone_info->fill,
    exception);
  status=QueryColorCompliance("#00000000",AllCompliance,&clone_info->stroke,
    exception);
  clone_info->stroke_width=0.0;
  clone_info->alpha=OpaqueAlpha;
  status=RenderMVGContent(composite_mask,clone_info,0,exception);
  clone_info=DestroyDrawInfo(clone_info);
  separate_mask=SeparateImage(composite_mask,AlphaChannel,exception);
  if (separate_mask != (Image *) NULL)
    {
      composite_mask=DestroyImage(composite_mask);
      composite_mask=separate_mask;
      status=NegateImage(composite_mask,MagickFalse,exception);
      if (status == MagickFalse)
        composite_mask=DestroyImage(composite_mask);
    }
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(DrawEvent,GetMagickModule(),"end mask-path");
  return(composite_mask);
}