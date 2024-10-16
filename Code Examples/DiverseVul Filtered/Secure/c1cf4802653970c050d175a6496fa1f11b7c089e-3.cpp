static MagickBooleanType IsPNM(const unsigned char *magick,const size_t extent)
{
  if (extent < 2)
    return(MagickFalse);
  if ((*magick == (unsigned char) 'P') &&
      ((magick[1] == '1') || (magick[1] == '2') || (magick[1] == '3') ||
       (magick[1] == '4') || (magick[1] == '5') || (magick[1] == '6') ||
       (magick[1] == '7') || (magick[1] == 'F') || (magick[1] == 'f')))
    return(MagickTrue);
  return(MagickFalse);
}