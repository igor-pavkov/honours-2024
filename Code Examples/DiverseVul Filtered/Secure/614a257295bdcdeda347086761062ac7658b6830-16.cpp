MagickExport unsigned char *ImageToBlob(const ImageInfo *image_info,
  Image *image,size_t *length,ExceptionInfo *exception)
{
  const MagickInfo
    *magick_info;

  ImageInfo
    *blob_info;

  MagickBooleanType
    status;

  unsigned char
    *blob;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  assert(exception != (ExceptionInfo *) NULL);
  *length=0;
  blob=(unsigned char *) NULL;
  blob_info=CloneImageInfo(image_info);
  blob_info->adjoin=MagickFalse;
  (void) SetImageInfo(blob_info,1,exception);
  if (*blob_info->magick != '\0')
    (void) CopyMagickString(image->magick,blob_info->magick,MaxTextExtent);
  magick_info=GetMagickInfo(image->magick,exception);
  if (magick_info == (const MagickInfo *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),
        MissingDelegateError,"NoDecodeDelegateForThisImageFormat","`%s'",
        image->magick);
      blob_info=DestroyImageInfo(blob_info);
      return(blob);
    }
  (void) CopyMagickString(blob_info->magick,image->magick,MaxTextExtent);
  if (GetMagickBlobSupport(magick_info) != MagickFalse)
    {
      /*
        Native blob support for this image format.
      */
      blob_info->length=0;
      blob_info->blob=AcquireQuantumMemory(MagickMaxBlobExtent,
        sizeof(unsigned char));
      if (blob_info->blob == (void *) NULL)
        (void) ThrowMagickException(exception,GetMagickModule(),
          ResourceLimitError,"MemoryAllocationFailed","`%s'",image->filename);
      else
        {
          (void) CloseBlob(image);
          image->blob->exempt=MagickTrue;
          *image->filename='\0';
          status=WriteImage(blob_info,image);
          InheritException(exception,&image->exception);
          *length=image->blob->length;
          blob=DetachBlob(image->blob);
          if (blob == (unsigned char *) NULL)
            blob_info->blob=RelinquishMagickMemory(blob_info->blob);
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
        unique[MaxTextExtent];

      int
        file;

      /*
        Write file to disk in blob image format.
      */
      file=AcquireUniqueFileResource(unique);
      if (file == -1)
        {
          ThrowFileException(exception,BlobError,"UnableToWriteBlob",
            image_info->filename);
        }
      else
        {
          blob_info->file=fdopen(file,"wb");
          if (blob_info->file != (FILE *) NULL)
            {
              (void) FormatLocaleString(image->filename,MaxTextExtent,"%s:%s",
                image->magick,unique);
              status=WriteImage(blob_info,image);
              (void) CloseBlob(image);
              (void) fclose(blob_info->file);
              if (status == MagickFalse)
                InheritException(exception,&image->exception);
              else
                blob=FileToBlob(unique,~0UL,length,exception);
            }
          (void) RelinquishUniqueFileResource(unique);
        }
    }
  blob_info=DestroyImageInfo(blob_info);
  return(blob);
}