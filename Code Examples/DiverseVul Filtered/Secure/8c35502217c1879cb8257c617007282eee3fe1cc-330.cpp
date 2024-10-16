void Magick::Image::annotate(const std::string &text_,
  const GravityType gravity_)
{
  DrawInfo
    *drawInfo;

  modifyImage();

  drawInfo=options()->drawInfo();
  drawInfo->text=DestroyString(drawInfo->text);
  drawInfo->text=const_cast<char *>(text_.c_str());
  drawInfo->gravity=gravity_;

  GetPPException;
  AnnotateImage(image(),drawInfo,exceptionInfo);

  drawInfo->gravity=NorthWestGravity;
  drawInfo->text=(char *) NULL;

  ThrowImageException;
}