static MagickBooleanType GetProfilesFromResourceBlock(Image *image,
  const StringInfo *resource_block,ExceptionInfo *exception)
{
  const unsigned char
    *datum;

  register const unsigned char
    *p;

  size_t
    length;

  StringInfo
    *profile;

  unsigned char
    length_byte;

  size_t
    count;

  unsigned short
    id;

  datum=GetStringInfoDatum(resource_block);
  length=GetStringInfoLength(resource_block);
  for (p=datum; p < (datum+length-16); )
  {
    if (LocaleNCompare((char *) p,"8BIM",4) != 0)
      break;
    p+=4;
    p=ReadResourceShort(p,&id);
    p=ReadResourceByte(p,&length_byte);
    p+=length_byte;
    if (((length_byte+1) & 0x01) != 0)
      p++;
    if (p > (datum+length-4))
      break;
    p=ReadResourceLong(p,&count);
    if ((p > (datum+length-count)) || (count > length))
      break;
    switch (id)
    {
      case 0x03ed:
      {
        unsigned short
          resolution;

        /*
          Resolution.
        */
        p=ReadResourceShort(p,&resolution)+6;
        image->resolution.x=(double) resolution;
        p=ReadResourceShort(p,&resolution)+6;
        image->resolution.y=(double) resolution;
        break;
      }
      case 0x0404:
      {
        /*
          IPTC Profile
        */
        profile=AcquireStringInfo(count);
        SetStringInfoDatum(profile,p);
        (void) SetImageProfile(image,"iptc",profile,exception);
        profile=DestroyStringInfo(profile);
        p+=count;
        break;
      }
      case 0x040c:
      {
        /*
          Thumbnail.
        */
        p+=count;
        break;
      }
      case 0x040f:
      {
        /*
          ICC Profile.
        */
        profile=AcquireStringInfo(count);
        SetStringInfoDatum(profile,p);
        (void) SetImageProfile(image,"icc",profile,exception);
        profile=DestroyStringInfo(profile);
        p+=count;
        break;
      }
      case 0x0422:
      {
        /*
          EXIF Profile.
        */
        profile=AcquireStringInfo(count);
        SetStringInfoDatum(profile,p);
        (void) SetImageProfile(image,"exif",profile,exception);
        profile=DestroyStringInfo(profile);
        p+=count;
        break;
      }
      case 0x0424:
      {
        /*
          XMP Profile.
        */
        profile=AcquireStringInfo(count);
        SetStringInfoDatum(profile,p);
        (void) SetImageProfile(image,"xmp",profile,exception);
        profile=DestroyStringInfo(profile);
        p+=count;
        break;
      }
      default:
      {
        p+=count;
        break;
      }
    }
    if ((count & 0x01) != 0)
      p++;
  }
  return(MagickTrue);
}