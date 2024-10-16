void Magick::Image::magick(const std::string &magick_)
{
  size_t
    length;

  modifyImage();

  length=sizeof(image()->magick)-1;
  if (magick_.length() < length)
    length=magick_.length();

  if (!magick_.empty())
    magick_.copy(image()->magick,length);
  image()->magick[length]=0;

  options()->magick(magick_);
}