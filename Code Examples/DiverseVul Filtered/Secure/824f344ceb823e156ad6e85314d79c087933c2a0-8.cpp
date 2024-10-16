static MagickBooleanType TIFFGetProperties(TIFF *tiff,Image *image,
  ExceptionInfo *exception)
{
  char
    message[MagickPathExtent],
    *text;

  MagickBooleanType
    status;

  uint32
    count,
    type;

  text=(char *) NULL;
  status=MagickTrue;
  if ((TIFFGetField(tiff,TIFFTAG_ARTIST,&text) == 1) &&
      (text != (char *) NULL))
    status=SetImageProperty(image,"tiff:artist",text,exception);
  if ((TIFFGetField(tiff,TIFFTAG_COPYRIGHT,&text) == 1) &&
      (text != (char *) NULL))
    status=SetImageProperty(image,"tiff:copyright",text,exception);
  if ((TIFFGetField(tiff,TIFFTAG_DATETIME,&text) == 1) &&
      (text != (char *) NULL))
    status=SetImageProperty(image,"tiff:timestamp",text,exception);
  if ((TIFFGetField(tiff,TIFFTAG_DOCUMENTNAME,&text) == 1) &&
      (text != (char *) NULL))
    status=SetImageProperty(image,"tiff:document",text,exception);
  if ((TIFFGetField(tiff,TIFFTAG_HOSTCOMPUTER,&text) == 1) &&
      (text != (char *) NULL))
    status=SetImageProperty(image,"tiff:hostcomputer",text,exception);
  if ((TIFFGetField(tiff,TIFFTAG_IMAGEDESCRIPTION,&text) == 1) &&
      (text != (char *) NULL))
    status=SetImageProperty(image,"comment",text,exception);
  if ((TIFFGetField(tiff,TIFFTAG_MAKE,&text) == 1) &&
      (text != (char *) NULL))
    status=SetImageProperty(image,"tiff:make",text,exception);
  if ((TIFFGetField(tiff,TIFFTAG_MODEL,&text) == 1) &&
      (text != (char *) NULL))
    status=SetImageProperty(image,"tiff:model",text,exception);
  if ((TIFFGetField(tiff,TIFFTAG_OPIIMAGEID,&count,&text) == 1) &&
      (text != (char *) NULL))
    {
      if (count >= MagickPathExtent)
        count=MagickPathExtent-1;
      (void) CopyMagickString(message,text,count+1);
      status=SetImageProperty(image,"tiff:image-id",message,exception);
    }
  if ((TIFFGetField(tiff,TIFFTAG_PAGENAME,&text) == 1) &&
      (text != (char *) NULL))
    status=SetImageProperty(image,"label",text,exception);
  if ((TIFFGetField(tiff,TIFFTAG_SOFTWARE,&text) == 1) &&
      (text != (char *) NULL))
    status=SetImageProperty(image,"tiff:software",text,exception);
  if ((TIFFGetField(tiff,33423,&count,&text) == 1) && (text != (char *) NULL))
    {
      if (count >= MagickPathExtent)
        count=MagickPathExtent-1;
      (void) CopyMagickString(message,text,count+1);
      status=SetImageProperty(image,"tiff:kodak-33423",message,exception);
    }
  if ((TIFFGetField(tiff,36867,&count,&text) == 1) && (text != (char *) NULL))
    {
      if (count >= MagickPathExtent)
        count=MagickPathExtent-1;
      (void) CopyMagickString(message,text,count+1);
      status=SetImageProperty(image,"tiff:kodak-36867",message,exception);
    }
  if (TIFFGetField(tiff,TIFFTAG_SUBFILETYPE,&type) == 1)
    switch (type)
    {
      case 0x01:
      {
        status=SetImageProperty(image,"tiff:subfiletype","REDUCEDIMAGE",
          exception);
        break;
      }
      case 0x02:
      {
        status=SetImageProperty(image,"tiff:subfiletype","PAGE",exception);
        break;
      }
      case 0x04:
      {
        status=SetImageProperty(image,"tiff:subfiletype","MASK",exception);
        break;
      }
      default:
        break;
    }
  return(status);
}