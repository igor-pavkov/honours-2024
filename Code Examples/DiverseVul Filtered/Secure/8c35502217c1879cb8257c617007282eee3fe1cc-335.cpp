bool Magick::Image::hasChannel(const PixelChannel channel) const
{
  if (GetPixelChannelTraits(constImage(),channel) == UndefinedPixelTrait)
    return(false);

  if (channel == GreenPixelChannel || channel == BluePixelChannel)
    return (GetPixelChannelOffset(constImage(),channel) == (ssize_t)channel);

  return(true);
}