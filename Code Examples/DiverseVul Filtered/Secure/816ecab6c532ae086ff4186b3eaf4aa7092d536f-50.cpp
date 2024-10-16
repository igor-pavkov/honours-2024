static MagickBooleanType WriteMNGImage(const ImageInfo *image_info,
  Image *image)
{
  return(WritePNGImage(image_info,image));
}