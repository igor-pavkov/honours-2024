static size_t PingStream(const Image *magick_unused(image),
  const void *magick_unused(pixels),const size_t columns)
{
  magick_unreferenced(image);
  magick_unreferenced(pixels);

  return(columns);
}