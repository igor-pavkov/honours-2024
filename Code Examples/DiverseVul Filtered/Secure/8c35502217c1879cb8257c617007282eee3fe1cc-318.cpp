void Magick::Image::sparseColor(const ChannelType channel_,
  const SparseColorMethod method_,const size_t numberArguments_,
  const double *arguments_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  GetAndSetPPChannelMask(channel_);
  newImage=SparseColorImage(constImage(),method_,numberArguments_,arguments_,
    exceptionInfo);
  RestorePPChannelMask;
  replaceImage(newImage);
  ThrowImageException;
}