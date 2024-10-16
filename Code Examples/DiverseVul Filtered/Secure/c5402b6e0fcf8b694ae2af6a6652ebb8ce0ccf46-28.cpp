MagickPrivate double *GetResizeFilterCoefficient(
  const ResizeFilter *resize_filter)
{
  assert(resize_filter != (ResizeFilter *) NULL);
  assert(resize_filter->signature == MagickCoreSignature);
  return((double *) resize_filter->coefficient);
}