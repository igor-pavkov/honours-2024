static MagickBooleanType TIFFGetEXIFProperties(TIFF *tiff,Image *image)
{
#if defined(MAGICKCORE_HAVE_TIFFREADEXIFDIRECTORY)
  MagickBooleanType
    status;

  tdir_t
    directory;

#if defined(TIFF_VERSION_BIG)
  uint64
#else
  uint32
#endif
    offset;

  /*
    Read EXIF properties.
  */
  offset=0;
  if (TIFFGetField(tiff,TIFFTAG_EXIFIFD,&offset) != 1)
    return(MagickFalse);
  directory=TIFFCurrentDirectory(tiff);
  if (TIFFReadEXIFDirectory(tiff,offset) != 1)
    {
      TIFFSetDirectory(tiff,directory);
      return(MagickFalse);
    }
  status=TIFFSetImageProperties(tiff,image,"exif:");
  TIFFSetDirectory(tiff,directory);
  return(status);
#else
  (void) tiff;
  (void) image;
  return(MagickTrue);
#endif
}