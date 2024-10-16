void Magick::Image::write(Blob *blob_,const std::string &magick_)
{
  size_t
    length=0;

  void
    *data;

  modifyImage();
  magick(magick_);
  GetPPException;
  data=ImagesToBlob(constImageInfo(),image(),&length,exceptionInfo);
  if (length > 0)
    blob_->updateNoCopy(data,length,Blob::MallocAllocator);
  ThrowImageException;
}