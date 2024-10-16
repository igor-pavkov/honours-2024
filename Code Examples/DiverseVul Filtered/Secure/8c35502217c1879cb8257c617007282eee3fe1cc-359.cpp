void Magick::Image::texture(const Image &texture_)
{
  modifyImage();
  GetPPException;
  TextureImage(image(),texture_.constImage(),exceptionInfo);
  ThrowImageException;
}