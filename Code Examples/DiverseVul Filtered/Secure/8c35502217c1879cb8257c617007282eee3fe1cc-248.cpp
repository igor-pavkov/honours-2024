void Magick::Image::pixelColor(const ssize_t x_,const ssize_t y_,
  const Color &color_)
{
  PixelInfo
    packet;

  Quantum
    *pixel;

  // Test arguments to ensure they are within the image.
  if (y_ > (ssize_t) rows() || x_ > (ssize_t) columns())
    throwExceptionExplicit(MagickCore::OptionError,
      "Access outside of image boundary");

  modifyImage();

  // Set image to DirectClass
  classType(DirectClass );

  // Get pixel view
  Pixels pixels(*this);
    // Set pixel value
  pixel=pixels.get(x_, y_, 1, 1 );
  packet=color_;
  MagickCore::SetPixelViaPixelInfo(constImage(),&packet,pixel);
  // Tell ImageMagick that pixels have been updated
  pixels.sync();
}