MagickExport Image *ThumbnailImage(const Image *image,const size_t columns,
  const size_t rows,ExceptionInfo *exception)
{
#define SampleFactor  5

  char
    filename[MagickPathExtent],
    value[MagickPathExtent];

  const char
    *name;

  Image
    *thumbnail_image;

  double
    x_factor,
    y_factor;

  struct stat
    attributes;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  x_factor=(double) columns/(double) image->columns;
  y_factor=(double) rows/(double) image->rows;
  if ((x_factor*y_factor) > 0.1)
    thumbnail_image=ResizeImage(image,columns,rows,image->filter,exception);
  else
    if (((SampleFactor*columns) < 128) || ((SampleFactor*rows) < 128))
      thumbnail_image=ResizeImage(image,columns,rows,image->filter,exception);
    else
      {
        Image
          *sample_image;

        sample_image=SampleImage(image,SampleFactor*columns,SampleFactor*rows,
          exception);
        if (sample_image == (Image *) NULL)
          return((Image *) NULL);
        thumbnail_image=ResizeImage(sample_image,columns,rows,image->filter,
          exception);
        sample_image=DestroyImage(sample_image);
      }
  if (thumbnail_image == (Image *) NULL)
    return(thumbnail_image);
  (void) ParseAbsoluteGeometry("0x0+0+0",&thumbnail_image->page);
  if (thumbnail_image->alpha_trait == UndefinedPixelTrait)
    (void) SetImageAlphaChannel(thumbnail_image,OpaqueAlphaChannel,exception);
  thumbnail_image->depth=8;
  thumbnail_image->interlace=NoInterlace;
  /*
    Strip all profiles except color profiles.
  */
  ResetImageProfileIterator(thumbnail_image);
  for (name=GetNextImageProfile(thumbnail_image); name != (const char *) NULL; )
  {
    if ((LocaleCompare(name,"icc") != 0) && (LocaleCompare(name,"icm") != 0))
     {
       (void) DeleteImageProfile(thumbnail_image,name);
       ResetImageProfileIterator(thumbnail_image);
     }
    name=GetNextImageProfile(thumbnail_image);
  }
  (void) DeleteImageProperty(thumbnail_image,"comment");
  (void) CopyMagickString(value,image->magick_filename,MagickPathExtent);
  if (strstr(image->magick_filename,"//") == (char *) NULL)
    (void) FormatLocaleString(value,MagickPathExtent,"file://%s",
      image->magick_filename);
  (void) SetImageProperty(thumbnail_image,"Thumb::URI",value,exception);
  GetPathComponent(image->magick_filename,TailPath,filename);
  (void) CopyMagickString(value,filename,MagickPathExtent);
  if ( GetPathAttributes(image->filename,&attributes) != MagickFalse )
    {
      (void) FormatLocaleString(value,MagickPathExtent,"%.20g",(double)
        attributes.st_mtime);
      (void) SetImageProperty(thumbnail_image,"Thumb::MTime",value,exception);
    }
  (void) FormatLocaleString(value,MagickPathExtent,"%.20g",(double)
    attributes.st_mtime);
  (void) FormatMagickSize(GetBlobSize(image),MagickFalse,"B",MagickPathExtent,
    value);
  (void) SetImageProperty(thumbnail_image,"Thumb::Size",value,exception);
  (void) FormatLocaleString(value,MagickPathExtent,"image/%s",image->magick);
  LocaleLower(value);
  (void) SetImageProperty(thumbnail_image,"Thumb::Mimetype",value,exception);
  (void) SetImageProperty(thumbnail_image,"software",MagickAuthoritativeURL,
    exception);
  (void) FormatLocaleString(value,MagickPathExtent,"%.20g",(double)
    image->magick_columns);
  (void) SetImageProperty(thumbnail_image,"Thumb::Image::Width",value,
    exception);
  (void) FormatLocaleString(value,MagickPathExtent,"%.20g",(double)
    image->magick_rows);
  (void) SetImageProperty(thumbnail_image,"Thumb::Image::Height",value,
    exception);
  (void) FormatLocaleString(value,MagickPathExtent,"%.20g",(double)
    GetImageListLength(image));
  (void) SetImageProperty(thumbnail_image,"Thumb::Document::Pages",value,
    exception);
  return(thumbnail_image);
}