void Magick::Image::morphologyChannel(const ChannelType channel_,
  const MorphologyMethod method_,const std::string kernel_,
  const ssize_t iterations_)
{
  KernelInfo
    *kernel;

  MagickCore::Image
    *newImage;


  GetPPException;
  kernel=AcquireKernelInfo(kernel_.c_str(),exceptionInfo);
  if (kernel == (KernelInfo *)NULL)
    {
      throwExceptionExplicit(MagickCore::OptionError,
        "Unable to parse kernel.");
      return;
    }
  GetAndSetPPChannelMask(channel_);
  newImage=MorphologyImage(constImage(),method_,iterations_,kernel,
    exceptionInfo);
  RestorePPChannelMask;
  replaceImage(newImage);
  kernel=DestroyKernelInfo(kernel);
  ThrowImageException;
}