void Magick::Image::profile(const std::string name_,
  const Magick::Blob &profile_)
{
  modifyImage();
  GetPPException;
  (void) ProfileImage(image(),name_.c_str(),(unsigned char *)profile_.data(),
    profile_.length(),exceptionInfo);
  ThrowImageException;
}