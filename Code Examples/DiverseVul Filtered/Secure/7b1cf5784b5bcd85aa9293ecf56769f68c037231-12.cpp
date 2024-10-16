MagickExport MagickBooleanType TransformImages(Image **images,
  const char *crop_geometry,const char *image_geometry,ExceptionInfo *exception)
{
  Image
    *image,
    **image_list,
    *transform_images;

  MagickStatusType
    status;

  register ssize_t
    i;

  assert(images != (Image **) NULL);
  assert((*images)->signature == MagickCoreSignature);
  if ((*images)->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      (*images)->filename);
  image_list=ImageListToArray(*images,exception);
  if (image_list == (Image **) NULL)
    return(MagickFalse);
  status=MagickTrue;
  transform_images=NewImageList();
  for (i=0; image_list[i] != (Image *) NULL; i++)
  {
    image=image_list[i];
    status&=TransformImage(&image,crop_geometry,image_geometry,exception);
    AppendImageToList(&transform_images,image);
  }
  *images=transform_images;
  image_list=(Image **) RelinquishMagickMemory(image_list);
  return(status != 0 ? MagickTrue : MagickFalse);
}