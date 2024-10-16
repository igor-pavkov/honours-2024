void Magick::Image::process(std::string name_,const ssize_t argc,
  const char **argv)
{
  modifyImage();

  GetPPException;
  (void) InvokeDynamicImageFilter(name_.c_str(),&image(),argc,argv,
      exceptionInfo);
  ThrowImageException;
}