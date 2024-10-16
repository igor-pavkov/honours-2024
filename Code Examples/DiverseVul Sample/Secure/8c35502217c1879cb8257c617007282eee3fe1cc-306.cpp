void Magick::Image::modulusDepth(const size_t depth_)
{
  modifyImage();
  GetPPException;
  SetImageDepth(image(),depth_,exceptionInfo);
  ThrowImageException;
  options()->depth(depth_);
}