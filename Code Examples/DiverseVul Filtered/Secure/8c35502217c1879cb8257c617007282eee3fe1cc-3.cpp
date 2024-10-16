void Magick::Image::perceptible(const double epsilon_)
{
  modifyImage();
  GetPPException;
  PerceptibleImage(image(),epsilon_,exceptionInfo);
  ThrowImageException;
}