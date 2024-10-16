MagickExport Image *UniqueImageColors(const Image *image,
  ExceptionInfo *exception)
{
  CacheView
    *unique_view;

  CubeInfo
    *cube_info;

  Image
    *unique_image;

  cube_info=ClassifyImageColors(image,exception);
  if (cube_info == (CubeInfo *) NULL)
    return((Image *) NULL);
  unique_image=CloneImage(image,cube_info->colors,1,MagickTrue,exception);
  if (unique_image == (Image *) NULL)
    return(unique_image);
  if (SetImageStorageClass(unique_image,DirectClass,exception) == MagickFalse)
    {
      unique_image=DestroyImage(unique_image);
      return((Image *) NULL);
    }
  unique_view=AcquireAuthenticCacheView(unique_image,exception);
  UniqueColorsToImage(unique_image,unique_view,cube_info,cube_info->root,
    exception);
  unique_view=DestroyCacheView(unique_view);
  cube_info=DestroyCubeInfo(image,cube_info);
  return(unique_image);
}