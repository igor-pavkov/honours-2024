Magick::Image Magick::Image::strokePattern(void) const
{
  // FIXME: This is inordinately innefficient
  const MagickCore::Image 
    *tmpTexture;

  Image
    texture;

  tmpTexture=constOptions()->strokePattern();

  if (tmpTexture)
    {
      MagickCore::Image
        *image;

      GetPPException;
      image=CloneImage(tmpTexture,0,0,MagickTrue,exceptionInfo);
      texture.replaceImage(image);
      ThrowImageException;
    }
  return(texture);
}