bool Magick::Image::alpha(void) const
{
  if (constImage()->alpha_trait == BlendPixelTrait)
    return(true);
  else
    return(false);
}