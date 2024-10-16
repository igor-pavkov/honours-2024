static void InitPSDInfo(Image *image,Image *layer,PSDInfo *info)
{
  info->version=1;
  info->columns=layer->columns;
  info->rows=layer->rows;
  info->mode=10; /* Set mode to a value that won't change the colorspace */
  /* Assume that image has matte */
  if (IsGrayImage(image,&image->exception) != MagickFalse)
    info->channels=2U;
  else
    if (image->storage_class == PseudoClass)
      {
        info->mode=2;
        info->channels=2U;
      }
    else
      {
        if (image->colorspace != CMYKColorspace)
          info->channels=4U;
        else
          info->channels=5U;
      }
  if (image->matte == MagickFalse)
    info->channels--;
  info->min_channels=info->channels;
  if (image->matte != MagickFalse)
    info->min_channels--;
}