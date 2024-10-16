MagickCore::Image *Magick::Image::replaceImage(MagickCore::Image *replacement_)
{
  MagickCore::Image
    *image;

  if (replacement_)
    image=replacement_;
  else
    {
      GetPPException;
      image=AcquireImage(constImageInfo(),exceptionInfo);
      ThrowImageException;
    }

  _imgRef=ImageRef::replaceImage(_imgRef,image);
  return(image);
}