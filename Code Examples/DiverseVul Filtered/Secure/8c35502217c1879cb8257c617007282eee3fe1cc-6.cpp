void Magick::Image::depth(const size_t depth_)
{
  size_t
    depth = depth_;

  if (depth > MAGICKCORE_QUANTUM_DEPTH)
    depth=MAGICKCORE_QUANTUM_DEPTH;

  modifyImage();
  image()->depth=depth;
  options()->depth(depth);
}