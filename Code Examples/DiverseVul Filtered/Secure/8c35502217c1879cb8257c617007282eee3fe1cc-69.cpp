Magick::RenderingIntent Magick::Image::renderingIntent(void) const
{
  return(static_cast<Magick::RenderingIntent>(constImage()->rendering_intent));
}