void Magick::Image::gifDisposeMethod(
  const MagickCore::DisposeType disposeMethod_)
{
  modifyImage();
  image()->dispose=disposeMethod_;
}