void Magick::Image::levelColorsChannel(const ChannelType channel_,
  const Color &blackColor_,const Color &whiteColor_,const bool invert_)
{
  PixelInfo
    black,
    white;

  modifyImage();

  black=static_cast<PixelInfo>(blackColor_);
  white=static_cast<PixelInfo>(whiteColor_);
  GetPPException;
  GetAndSetPPChannelMask(channel_);
  (void) LevelImageColors(image(),&black,&white,invert_ == true ?
    MagickTrue : MagickFalse,exceptionInfo);
  RestorePPChannelMask;
  ThrowImageException;
}