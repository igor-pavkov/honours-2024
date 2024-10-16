MagickPrivate ResizeFilter *DestroyResizeFilter(ResizeFilter *resize_filter)
{
  assert(resize_filter != (ResizeFilter *) NULL);
  assert(resize_filter->signature == MagickCoreSignature);
  resize_filter->signature=(~MagickCoreSignature);
  resize_filter=(ResizeFilter *) RelinquishMagickMemory(resize_filter);
  return(resize_filter);
}