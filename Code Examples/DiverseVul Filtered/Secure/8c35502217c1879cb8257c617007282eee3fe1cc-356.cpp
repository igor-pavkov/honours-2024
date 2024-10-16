void Magick::Image::alpha(const bool alphaFlag_)
{
  modifyImage();

  // If matte channel is requested, but image doesn't already have a
  // matte channel, then create an opaque matte channel.  Likewise, if
  // the image already has a matte channel but a matte channel is not
  // desired, then set the matte channel to opaque.
  GetPPException;
  if ((alphaFlag_ && !constImage()->alpha_trait) ||
      (constImage()->alpha_trait && !alphaFlag_))
    SetImageAlpha(image(),OpaqueAlpha,exceptionInfo);
  ThrowImageException;

  image()->alpha_trait=alphaFlag_ ? BlendPixelTrait : UndefinedPixelTrait;
}