void Magick::Image::distort(const DistortMethod method_,
  const size_t numberArguments_,const double *arguments_,const bool bestfit_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=DistortImage(constImage(), method_,numberArguments_,arguments_,
    bestfit_ == true ? MagickTrue : MagickFalse,exceptionInfo);
  replaceImage(newImage);
  ThrowImageException;
}