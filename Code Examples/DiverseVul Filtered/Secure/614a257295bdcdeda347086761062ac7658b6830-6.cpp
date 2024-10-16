MagickExport BlobInfo *CloneBlobInfo(const BlobInfo *blob_info)
{
  BlobInfo
    *clone_info;

  SemaphoreInfo
    *semaphore;

  clone_info=(BlobInfo *) AcquireMagickMemory(sizeof(*clone_info));
  GetBlobInfo(clone_info);
  if (blob_info == (BlobInfo *) NULL)
    return(clone_info);
  semaphore=clone_info->semaphore;
  (void) memcpy(clone_info,blob_info,sizeof(*clone_info));
  if (blob_info->mapped != MagickFalse)
    (void) AcquireMagickResource(MapResource,blob_info->length);
  clone_info->semaphore=semaphore;
  LockSemaphoreInfo(clone_info->semaphore);
  clone_info->reference_count=1;
  UnlockSemaphoreInfo(clone_info->semaphore);
  return(clone_info);
}