static MagickBooleanType Magick_png_write_chunk_from_profile(Image *image,
  const char *string, MagickBooleanType logging)
{
  char
    *name;

  const StringInfo
    *profile;

  unsigned char
    *data;

  png_uint_32 length;

  ResetImageProfileIterator(image);

  for (name=GetNextImageProfile(image); name != (const char *) NULL; )
  {
    profile=GetImageProfile(image,name);

    if (profile != (const StringInfo *) NULL)
      {
        StringInfo
          *ping_profile;

        if (LocaleNCompare(name,string,11) == 0)
          {
            if (logging != MagickFalse)
               (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                   "  Found %s profile",name);

            ping_profile=CloneStringInfo(profile);
            data=GetStringInfoDatum(ping_profile),
            length=(png_uint_32) GetStringInfoLength(ping_profile);
            data[4]=data[3];
            data[3]=data[2];
            data[2]=data[1];
            data[1]=data[0];
            (void) WriteBlobMSBULong(image,length-5);  /* data length */
            (void) WriteBlob(image,length-1,data+1);
            (void) WriteBlobMSBULong(image,crc32(0,data+1,(uInt) length-1));
            ping_profile=DestroyStringInfo(ping_profile);
          }
      }

      name=GetNextImageProfile(image);
   }

   return(MagickTrue);
}