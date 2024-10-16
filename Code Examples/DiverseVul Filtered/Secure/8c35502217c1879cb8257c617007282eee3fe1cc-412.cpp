void Magick::Image::iptcProfile(const Magick::Blob &iptcProfile_)
{
  modifyImage();
  if (iptcProfile_.data() != 0)
    {
      StringInfo
        *iptc_profile;

      iptc_profile=AcquireStringInfo(iptcProfile_.length());
      SetStringInfoDatum(iptc_profile,(unsigned char *) iptcProfile_.data());
      GetPPException;
      (void) SetImageProfile(image(),"iptc",iptc_profile,exceptionInfo);
      iptc_profile=DestroyStringInfo(iptc_profile);
      ThrowImageException;
    }
}