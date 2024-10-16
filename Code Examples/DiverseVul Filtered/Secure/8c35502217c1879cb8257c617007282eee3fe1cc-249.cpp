void Magick::Image::write(Blob *blob_)
{
  size_t
    length=0;

  void
    *data;

  modifyImage();
  GetPPException;
  data=ImagesToBlob(constImageInfo(),image(),&length,exceptionInfo);
  if (length > 0)
    blob_->updateNoCopy(data,length,Blob::MallocAllocator);
  ThrowImageException;
}