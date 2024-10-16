static MagickBooleanType TIFFSetImageProperties(TIFF *tiff,Image *image,
  const char *tag)
{
  char
    buffer[MagickPathExtent],
    filename[MagickPathExtent];

  FILE
    *file;

  int
    unique_file;

  /*
    Set EXIF or GPS image properties.
  */
  unique_file=AcquireUniqueFileResource(filename);
  file=(FILE *) NULL;
  if (unique_file != -1)
    file=fdopen(unique_file,"rb+");
  if ((unique_file == -1) || (file == (FILE *) NULL))
    {
      (void) RelinquishUniqueFileResource(filename);
      (void) ThrowMagickException(&image->exception,GetMagickModule(),WandError,
        "UnableToCreateTemporaryFile","`%s'",filename);
      return(MagickFalse);
    }
  TIFFPrintDirectory(tiff,file,0);
  (void) fseek(file,0,SEEK_SET);
  while (fgets(buffer,(int) sizeof(buffer),file) != NULL)
  {
    char
      *p,
      property[MagickPathExtent],
      value[MagickPathExtent];

    StripString(buffer);
    p=strchr(buffer,':');
    if (p == (char *) NULL)
      continue;
    *p='\0';
    (void) sprintf(property,"%s%.1024s",tag,buffer);
    (void) sprintf(value,"%s",p+1);
    StripString(value);
    (void) SetImageProperty(image,property,value);
  }
  (void) fclose(file);
  (void) RelinquishUniqueFileResource(filename);
  return(MagickTrue);
}