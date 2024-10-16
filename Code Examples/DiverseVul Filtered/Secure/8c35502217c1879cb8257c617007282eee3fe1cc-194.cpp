std::string Magick::Image::formatExpression(const std::string expression)
{
  char
    *text;

  std::string
    text_string;

  GetPPException;
  modifyImage();
  text=InterpretImageProperties(imageInfo(),image(),expression.c_str(),
    exceptionInfo);
  if (text != (char *) NULL)
    {
      text_string=std::string(text);
      text=DestroyString(text);
    }
  ThrowImageException;
  return(text_string);
}