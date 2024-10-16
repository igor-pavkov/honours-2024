MagickExport unsigned char *ImagesToBlob(const ImageInfo *image_info,
  Image *images,size_t *length,ExceptionInfo *exception)
{
  const MagickInfo
    *magick_info;

  ImageInfo
    *clone_info;

  MagickBooleanType
    status;

  unsigned char
    *blob;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(images != (Image *) NULL);
  assert(images->signature == MagickCoreSignature);
  assert(exception != (ExceptionInfo *) NULL);
  *length=0;
  blob=(unsigned char *) NULL;
  clone_info=CloneImageInfo(image_info);
  (void) SetImageInfo(clone_info,(unsigned int) GetImageListLength(images),
    exception);
  if (*clone_info->magick != '\0')
    (void) CopyMagickString(images->magick,clone_info->magick,MaxTextExtent);
  magick_info=GetMagickInfo(images->magick,exception);
  if (magick_info == (const MagickInfo *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),
        MissingDelegateError,"NoDecodeDelegateForThisImageFormat","`%s'",
        images->magick);
      clone_info=DestroyImageInfo(clone_info);
      return(blob);
    }
  if (GetMagickAdjoin(magick_info) == MagickFalse)
    {
      clone_info=DestroyImageInfo(clone_info);
      return(ImageToBlob(image_info,images,length,exception));
    }
  (void) CopyMagickString(clone_info->magick,images->magick,MaxTextExtent);
  if (GetMagickBlobSupport(magick_info) != MagickFalse)
    {
      /*
        Native blob support for this images format.
      */
      clone_info->length=0;
      clone_info->blob=AcquireQuantumMemory(MagickMaxBlobExtent,
        sizeof(unsigned char));
      if (clone_info->blob == (void *) NULL)
        (void) ThrowMagickException(exception,GetMagickModule(),
          ResourceLimitError,"MemoryAllocationFailed","`%s'",images->filename);
      else
        {
          (void) CloseBlob(images);
          images->blob->exempt=MagickTrue;
          *images->filename='\0';
          status=WriteImages(clone_info,images,images->filename,exception);
          *length=images->blob->length;
          blob=DetachBlob(images->blob);
          if (blob == (unsigned char *) NULL)
            clone_info->blob=RelinquishMagickMemory(clone_info->blob);
          else if (status == MagickFalse)
            blob=(unsigned char *) RelinquishMagickMemory(blob);
          else
            blob=(unsigned char *) ResizeQuantumMemory(blob,*length+1,
              sizeof(*blob));
        }
    }
  else
    {
      char
        filename[MaxTextExtent],
        unique[MaxTextExtent];

      int
        file;

      /*
        Write file to disk in blob images format.
      */
      file=AcquireUniqueFileResource(unique);
      if (file == -1)
        {
          ThrowFileException(exception,FileOpenError,"UnableToWriteBlob",
            image_info->filename);
        }
      else
        {
          clone_info->file=fdopen(file,"wb");
          if (clone_info->file != (FILE *) NULL)
            {
              (void) FormatLocaleString(filename,MaxTextExtent,"%s:%s",
                images->magick,unique);
              status=WriteImages(clone_info,images,filename,exception);
              (void) CloseBlob(images);
              (void) fclose(clone_info->file);
              if (status == MagickFalse)
                InheritException(exception,&images->exception);
              else
                blob=FileToBlob(unique,~0UL,length,exception);
            }
          (void) RelinquishUniqueFileResource(unique);
        }
    }
  clone_info=DestroyImageInfo(clone_info);
  return(blob);
}