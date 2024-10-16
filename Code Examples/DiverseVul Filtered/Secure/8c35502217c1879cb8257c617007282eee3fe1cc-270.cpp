void Magick::Image::decipher(const std::string &passphrase_)
{
  modifyImage();
  GetPPException;
  DecipherImage(image(),passphrase_.c_str(),exceptionInfo);
  ThrowImageException;
}