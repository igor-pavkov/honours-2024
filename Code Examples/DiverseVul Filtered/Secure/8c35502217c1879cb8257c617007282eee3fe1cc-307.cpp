size_t Magick::Image::modulusDepth(void) const
{
  size_t 
    depth;

  GetPPException;
  depth=GetImageDepth(constImage(),exceptionInfo);
  ThrowImageException;
  return(depth);
}