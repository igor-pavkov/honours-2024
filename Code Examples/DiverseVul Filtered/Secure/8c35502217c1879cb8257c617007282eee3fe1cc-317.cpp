void Magick::Image::connectedComponents(const size_t connectivity_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=ConnectedComponentsImage(constImage(),connectivity_,
    (CCObjectInfo **) NULL,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}