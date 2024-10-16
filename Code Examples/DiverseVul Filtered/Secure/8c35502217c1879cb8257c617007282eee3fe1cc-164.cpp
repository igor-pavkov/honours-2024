void Magick::Image::textGravity(GravityType gravity_)
{
  modifyImage();
  options()->textGravity(gravity_);
}