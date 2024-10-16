Magick::Color Magick::Image::pixelColor(const ssize_t x_,
  const ssize_t y_) const
{
  const Quantum
    *pixel;

  pixel=getConstPixels(x_,y_,1,1);
  if (pixel)
    {
      PixelInfo
        packet;

      MagickCore::GetPixelInfoPixel(constImage(),pixel,&packet);
      return(Color(packet));
    }

  return(Color()); // invalid
}