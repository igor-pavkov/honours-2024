void Magick::Image::morphologyChannel(const ChannelType channel_,
  const MorphologyMethod method_,const KernelInfoType kernel_,
  const std::string arguments_,const ssize_t iterations_)
{
  const char
    *option;

  std::string
    kernel;

  option=CommandOptionToMnemonic(MagickKernelOptions,kernel_);
  if (option == (const char *)NULL)
    {
      throwExceptionExplicit(MagickCore::OptionError,
        "Unable to determine kernel type.");
      return;
    }

  kernel=std::string(option);
  if (!arguments_.empty())
    kernel+=":"+arguments_;

  morphologyChannel(channel_,method_,kernel,iterations_);
}