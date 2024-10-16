static void TIFFReadPhotoshopLayers(Image* image,const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  const char
    *option;

  const StringInfo
    *profile;

  CustomStreamInfo
    *custom_stream;

  Image
    *layers;

  PhotoshopProfile
    photoshop_profile;

  PSDInfo
    info;

  register ssize_t
    i;

  if (GetImageListLength(image) != 1)
    return;
  if ((image_info->number_scenes == 1) && (image_info->scene == 0))
    return;
  option=GetImageOption(image_info,"tiff:ignore-layers");
  if (option != (const char * ) NULL)
    return;
  profile=GetImageProfile(image,"tiff:37724");
  if (profile == (const StringInfo *) NULL)
    return;
  for (i=0; i < (ssize_t) profile->length-8; i++)
  {
    if (LocaleNCompare((const char *) (profile->datum+i),
        image->endian == MSBEndian ? "8BIM" : "MIB8",4) != 0)
      continue;
    i+=4;
    if ((LocaleNCompare((const char *) (profile->datum+i),
         image->endian == MSBEndian ? "Layr" : "ryaL",4) == 0) ||
        (LocaleNCompare((const char *) (profile->datum+i),
         image->endian == MSBEndian ? "LMsk" : "ksML",4) == 0) ||
        (LocaleNCompare((const char *) (profile->datum+i),
         image->endian == MSBEndian ? "Lr16" : "61rL",4) == 0) ||
        (LocaleNCompare((const char *) (profile->datum+i),
         image->endian == MSBEndian ? "Lr32" : "23rL",4) == 0))
      break;
  }
  i+=4;
  if (i >= (ssize_t) (profile->length-8))
    return;
  photoshop_profile.data=(StringInfo *) profile;
  photoshop_profile.length=profile->length;
  custom_stream=TIFFAcquireCustomStreamForReading(&photoshop_profile,exception);
  if (custom_stream == (CustomStreamInfo *) NULL)
    return;
  layers=CloneImage(image,image->columns,image->rows,MagickTrue,exception);
  if (layers == (Image *) NULL)
    {
      custom_stream=DestroyCustomStreamInfo(custom_stream);
      return;
    }
  (void) DeleteImageProfile(layers,"tiff:37724");
  AttachCustomStream(layers->blob,custom_stream);
  SeekBlob(layers,(MagickOffsetType) i,SEEK_SET);
  InitPSDInfo(layers,&info);
  (void) ReadPSDLayers(layers,image_info,&info,exception);
  DeleteImageFromList(&layers);
  if (layers != (Image *) NULL)
    {
      SetImageArtifact(image,"tiff:has-layers","true");
      AppendImageToList(&image,layers);
      while (layers != (Image *) NULL)
      {
        SetImageArtifact(layers,"tiff:has-layers","true");
        DetachBlob(layers->blob);
        layers=GetNextImageInList(layers);
      }
    }
  custom_stream=DestroyCustomStreamInfo(custom_stream);
}