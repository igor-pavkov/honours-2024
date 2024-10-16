static void TIFFSetProfiles(TIFF *tiff,Image *image)
{
  const char
    *name;

  const StringInfo
    *profile;

  if (image->profiles == (void *) NULL)
    return;
  ResetImageProfileIterator(image);
  for (name=GetNextImageProfile(image); name != (const char *) NULL; )
  {
    profile=GetImageProfile(image,name);
    if (GetStringInfoLength(profile) == 0)
      {
        name=GetNextImageProfile(image);
        continue;
      }
#if defined(TIFFTAG_XMLPACKET)
    if (LocaleCompare(name,"xmp") == 0)
      (void) TIFFSetField(tiff,TIFFTAG_XMLPACKET,(uint32) GetStringInfoLength(
        profile),GetStringInfoDatum(profile));
#endif
#if defined(TIFFTAG_ICCPROFILE)
    if (LocaleCompare(name,"icc") == 0)
      (void) TIFFSetField(tiff,TIFFTAG_ICCPROFILE,(uint32) GetStringInfoLength(
        profile),GetStringInfoDatum(profile));
#endif
    if (LocaleCompare(name,"iptc") == 0)
      {
        const TIFFField
          *field;

        size_t
          length;

        StringInfo
          *iptc_profile;

        iptc_profile=CloneStringInfo(profile);
        length=GetStringInfoLength(profile)+4-(GetStringInfoLength(profile) &
          0x03);
        SetStringInfoLength(iptc_profile,length);
        field=TIFFFieldWithTag(tiff,TIFFTAG_RICHTIFFIPTC);
        if (TIFFFieldDataType(field) == TIFF_LONG)
          {
            if (TIFFIsByteSwapped(tiff))
              TIFFSwabArrayOfLong((uint32 *) GetStringInfoDatum(iptc_profile),
                (unsigned long) (length/4));
            (void) TIFFSetField(tiff,TIFFTAG_RICHTIFFIPTC,(uint32)
              GetStringInfoLength(iptc_profile)/4,GetStringInfoDatum(
                iptc_profile));
          }
        else
          (void) TIFFSetField(tiff,TIFFTAG_RICHTIFFIPTC,(uint32)
            GetStringInfoLength(iptc_profile),GetStringInfoDatum(
              iptc_profile));
        iptc_profile=DestroyStringInfo(iptc_profile);
      }
#if defined(TIFFTAG_PHOTOSHOP)
    if (LocaleCompare(name,"8bim") == 0)
      (void) TIFFSetField(tiff,TIFFTAG_PHOTOSHOP,(uint32)
        GetStringInfoLength(profile),GetStringInfoDatum(profile));
#endif
    if (LocaleCompare(name,"tiff:37724") == 0)
      (void) TIFFSetField(tiff,37724,(uint32) GetStringInfoLength(profile),
        GetStringInfoDatum(profile));
    if (LocaleCompare(name,"tiff:34118") == 0)
      (void) TIFFSetField(tiff,34118,(uint32) GetStringInfoLength(profile),
        GetStringInfoDatum(profile));
    name=GetNextImageProfile(image);
  }
}