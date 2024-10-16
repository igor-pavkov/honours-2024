void Magick::Image::evaluate(const ChannelType channel_,
  const MagickFunction function_,const size_t number_parameters_,
  const double *parameters_)
{
  GetPPException;
  GetAndSetPPChannelMask(channel_);
  FunctionImage(image(),function_,number_parameters_,parameters_,
    exceptionInfo);
  RestorePPChannelMask;
  ThrowImageException;
}