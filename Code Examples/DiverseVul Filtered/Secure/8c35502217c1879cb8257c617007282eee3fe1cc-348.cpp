void Magick::Image::clipPath(const std::string pathname_,const bool inside_)
{
  modifyImage();
  GetPPException;
  ClipImagePath(image(),pathname_.c_str(),(MagickBooleanType) inside_,
    exceptionInfo);
  ThrowImageException;
}