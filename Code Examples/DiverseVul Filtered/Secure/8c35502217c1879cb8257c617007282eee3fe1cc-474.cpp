void Magick::Image::fileName(const std::string &fileName_)
{
  modifyImage();

  fileName_.copy(image()->filename,sizeof(image()->filename)-1);
  image()->filename[fileName_.length()]=0; // Null terminate

  options()->fileName(fileName_);
}