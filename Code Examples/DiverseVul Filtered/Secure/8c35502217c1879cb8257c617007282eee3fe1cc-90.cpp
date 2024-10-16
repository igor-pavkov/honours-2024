void Magick::Image::colorMap(const size_t index_,const Color &color_)
{
  MagickCore::Image
    *imageptr;

  imageptr=image();

  if (index_ > (MaxColormapSize-1))
    throwExceptionExplicit(MagickCore::OptionError,
      "Colormap index must be less than MaxColormapSize");

  if (!color_.isValid())
    throwExceptionExplicit(MagickCore::OptionError,
      "Color argument is invalid");

  modifyImage();

  // Ensure that colormap size is large enough
  if (colorMapSize() < (index_+1))
    colorMapSize(index_+1);

  // Set color at index in colormap
  (imageptr->colormap)[index_]=color_;
}