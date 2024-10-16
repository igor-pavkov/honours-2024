void Magick::Image::comment(const std::string &comment_)
{
  modifyImage();
  GetPPException;
  SetImageProperty(image(),"Comment",NULL,exceptionInfo);
  if (comment_.length() > 0)
    SetImageProperty(image(),"Comment",comment_.c_str(),exceptionInfo);
  ThrowImageException;
}