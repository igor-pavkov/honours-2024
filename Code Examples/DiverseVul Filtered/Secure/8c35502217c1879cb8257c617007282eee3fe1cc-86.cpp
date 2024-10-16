void Magick::Image::evaluate(const ChannelType channel_,
  const MagickEvaluateOperator operator_,double rvalue_)
{
  GetPPException;
  GetAndSetPPChannelMask(channel_);
  EvaluateImage(image(),operator_,rvalue_,exceptionInfo);
  RestorePPChannelMask;
  ThrowImageException;
}