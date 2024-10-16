void Magick::Image::exifProfile(const Magick::Blob &exifProfile_)
{
  modifyImage();

  if (exifProfile_.data() != 0)
    {
      StringInfo
        *exif_profile;

      exif_profile=AcquireStringInfo(exifProfile_.length());
      SetStringInfoDatum(exif_profile,(unsigned char *) exifProfile_.data());
      GetPPException;
      (void) SetImageProfile(image(),"exif",exif_profile,exceptionInfo);
      exif_profile=DestroyStringInfo(exif_profile);
      ThrowImageException;
    }
}