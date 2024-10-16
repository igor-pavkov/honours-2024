void Magick::Image::scene(const size_t scene_)
{
  modifyImage();
  image()->scene=scene_;
}