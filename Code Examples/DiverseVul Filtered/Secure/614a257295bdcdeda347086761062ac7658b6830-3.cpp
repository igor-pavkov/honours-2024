MagickPrivate void DisassociateBlob(Image *image)
{
  BlobInfo
    *magick_restrict blob_info,
    *clone_info;

  MagickBooleanType
    clone;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(image->blob != (BlobInfo *) NULL);
  assert(image->blob->signature == MagickCoreSignature);
  blob_info=image->blob;
  clone=MagickFalse;
  LockSemaphoreInfo(blob_info->semaphore);
  assert(blob_info->reference_count >= 0);
  if (blob_info->reference_count > 1)
    clone=MagickTrue;
  UnlockSemaphoreInfo(blob_info->semaphore);
  if (clone == MagickFalse)
    return;
  clone_info=CloneBlobInfo(blob_info);
  DestroyBlob(image);
  image->blob=clone_info;
}