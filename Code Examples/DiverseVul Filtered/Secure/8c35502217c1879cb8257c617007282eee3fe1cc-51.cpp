void Magick::Image::renderingIntent(
  const Magick::RenderingIntent renderingIntent_)
{
  modifyImage();
  image()->rendering_intent=renderingIntent_;
}