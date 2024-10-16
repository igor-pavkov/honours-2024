MagickExport void GetBlobInfo(BlobInfo *blob_info)
{
  assert(blob_info != (BlobInfo *) NULL);
  (void) memset(blob_info,0,sizeof(*blob_info));
  blob_info->type=UndefinedStream;
  blob_info->quantum=(size_t) MagickMaxBlobExtent;
  blob_info->properties.st_mtime=GetMagickTime();
  blob_info->properties.st_ctime=blob_info->properties.st_mtime;
  blob_info->debug=IsEventLogging();
  blob_info->reference_count=1;
  blob_info->semaphore=AllocateSemaphoreInfo();
  blob_info->signature=MagickCoreSignature;
}