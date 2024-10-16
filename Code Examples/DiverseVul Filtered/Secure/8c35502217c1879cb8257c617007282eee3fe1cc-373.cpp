void Magick::Image::encipher(const std::string &passphrase_)
{
  modifyImage();
  GetPPException;
  EncipherImage(image(),passphrase_.c_str(),exceptionInfo);
  ThrowImageException;
}