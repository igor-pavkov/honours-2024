MagickPrivate double GetResizeFilterSupport(const ResizeFilter *resize_filter)
{
  assert(resize_filter != (ResizeFilter *) NULL);
  assert(resize_filter->signature == MagickCoreSignature);
  return(resize_filter->support*resize_filter->blur);
}