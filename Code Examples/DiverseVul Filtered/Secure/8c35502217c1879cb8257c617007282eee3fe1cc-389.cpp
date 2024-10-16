void Magick::Image::artifact(const std::string &name_,const std::string &value_)
{
  modifyImage();
  (void) SetImageArtifact(image(),name_.c_str(),value_.c_str());
}