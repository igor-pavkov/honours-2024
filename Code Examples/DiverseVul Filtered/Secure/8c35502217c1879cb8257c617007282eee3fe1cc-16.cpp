void Magick::Image::ping(const Blob& blob_)
{
  MagickCore::Image
    *newImage;

  GetPPException;
  newImage=PingBlob(imageInfo(),blob_.data(),blob_.length(),exceptionInfo);
  read(newImage,exceptionInfo);
}