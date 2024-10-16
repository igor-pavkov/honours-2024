void Magick::Image::label(const std::string &label_)
{
  modifyImage();
  GetPPException;
  (void) SetImageProperty(image(),"Label",NULL,exceptionInfo);
  if (label_.length() > 0)
    (void) SetImageProperty(image(),"Label",label_.c_str(),exceptionInfo);
  ThrowImageException;
}