MagickExport void DestroyBlob(Image *image)
{
  BlobInfo
    *magick_restrict blob_info;

  MagickBooleanType
    destroy;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(image->blob != (BlobInfo *) NULL);
  assert(image->blob->signature == MagickCoreSignature);
  blob_info=image->blob;
  destroy=MagickFalse;
  LockSemaphoreInfo(blob_info->semaphore);
  blob_info->reference_count--;
  assert(blob_info->reference_count >= 0);
  if (blob_info->reference_count == 0)
    destroy=MagickTrue;
  UnlockSemaphoreInfo(blob_info->semaphore);
  if (destroy == MagickFalse)
    {
      image->blob=(BlobInfo *) NULL;
      return;
    }
  (void) CloseBlob(image);
  if (blob_info->mapped != MagickFalse)
    {
      (void) UnmapBlob(blob_info->data,blob_info->length);
      RelinquishMagickResource(MapResource,blob_info->length);
    }
  if (blob_info->semaphore != (SemaphoreInfo *) NULL)
    DestroySemaphoreInfo(&blob_info->semaphore);
  blob_info->signature=(~MagickCoreSignature);
  image->blob=(BlobInfo *) RelinquishMagickMemory(blob_info);
}