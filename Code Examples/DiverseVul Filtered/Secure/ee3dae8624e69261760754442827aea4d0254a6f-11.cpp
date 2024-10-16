MagickExport MagickBooleanType SetImageInfo(ImageInfo *image_info,
  const unsigned int frames,ExceptionInfo *exception)
{
  char
    component[MagickPathExtent],
    magic[MagickPathExtent],
    *q;

  const MagicInfo
    *magic_info;

  const MagickInfo
    *magick_info;

  ExceptionInfo
    *sans_exception;

  Image
    *image;

  MagickBooleanType
    status;

  register const char
    *p;

  ssize_t
    count;

  /*
    Look for 'image.format' in filename.
  */
  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  *component='\0';
  GetPathComponent(image_info->filename,SubimagePath,component);
  if (*component != '\0')
    {
      /*
        Look for scene specification (e.g. img0001.pcd[4]).
      */
      if (IsSceneGeometry(component,MagickFalse) == MagickFalse)
        {
          if (IsGeometry(component) != MagickFalse)
            (void) CloneString(&image_info->extract,component);
        }
      else
        {
          size_t
            first,
            last;

          (void) CloneString(&image_info->scenes,component);
          image_info->scene=StringToUnsignedLong(image_info->scenes);
          image_info->number_scenes=image_info->scene;
          p=image_info->scenes;
          for (q=(char *) image_info->scenes; *q != '\0'; p++)
          {
            while ((isspace((int) ((unsigned char) *p)) != 0) || (*p == ','))
              p++;
            first=(size_t) strtol(p,&q,10);
            last=first;
            while (isspace((int) ((unsigned char) *q)) != 0)
              q++;
            if (*q == '-')
              last=(size_t) strtol(q+1,&q,10);
            if (first > last)
              Swap(first,last);
            if (first < image_info->scene)
              image_info->scene=first;
            if (last > image_info->number_scenes)
              image_info->number_scenes=last;
            p=q;
          }
          image_info->number_scenes-=image_info->scene-1;
        }
    }
  *component='\0';
  if (*image_info->magick == '\0')
    GetPathComponent(image_info->filename,ExtensionPath,component);
#if defined(MAGICKCORE_ZLIB_DELEGATE)
  if (*component != '\0')
    if ((LocaleCompare(component,"gz") == 0) ||
        (LocaleCompare(component,"Z") == 0) ||
        (LocaleCompare(component,"svgz") == 0) ||
        (LocaleCompare(component,"wmz") == 0))
      {
        char
          path[MagickPathExtent];

        (void) CopyMagickString(path,image_info->filename,MagickPathExtent);
        path[strlen(path)-strlen(component)-1]='\0';
        GetPathComponent(path,ExtensionPath,component);
      }
#endif
#if defined(MAGICKCORE_BZLIB_DELEGATE)
  if (*component != '\0')
    if (LocaleCompare(component,"bz2") == 0)
      {
        char
          path[MagickPathExtent];

        (void) CopyMagickString(path,image_info->filename,MagickPathExtent);
        path[strlen(path)-strlen(component)-1]='\0';
        GetPathComponent(path,ExtensionPath,component);
      }
#endif
  image_info->affirm=MagickFalse;
  sans_exception=AcquireExceptionInfo();
  if ((*component != '\0') && (IsGlob(component) == MagickFalse))
    {
      MagickFormatType
        format_type;

      register ssize_t
        i;

      static const char
        *format_type_formats[] =
        {
          "AUTOTRACE",
          "BROWSE",
          "DCRAW",
          "EDIT",
          "LAUNCH",
          "MPEG:DECODE",
          "MPEG:ENCODE",
          "PRINT",
          "PS:ALPHA",
          "PS:CMYK",
          "PS:COLOR",
          "PS:GRAY",
          "PS:MONO",
          "SCAN",
          "SHOW",
          "WIN",
          (char *) NULL
        };

      /*
        User specified image format.
      */
      (void) CopyMagickString(magic,component,MagickPathExtent);
      LocaleUpper(magic);
      /*
        Look for explicit image formats.
      */
      format_type=UndefinedFormatType;
      magick_info=GetMagickInfo(magic,sans_exception);
      if ((magick_info != (const MagickInfo *) NULL) &&
          (magick_info->format_type != UndefinedFormatType))
        format_type=magick_info->format_type;
      i=0;
      while ((format_type == UndefinedFormatType) &&
             (format_type_formats[i] != (char *) NULL))
      {
        if ((*magic == *format_type_formats[i]) &&
            (LocaleCompare(magic,format_type_formats[i]) == 0))
          format_type=ExplicitFormatType;
        i++;
      }
      if (format_type == UndefinedFormatType)
        (void) CopyMagickString(image_info->magick,magic,MagickPathExtent);
      else
        if (format_type == ExplicitFormatType)
          {
            image_info->affirm=MagickTrue;
            (void) CopyMagickString(image_info->magick,magic,MagickPathExtent);
          }
      if (LocaleCompare(magic,"RGB") == 0)
        image_info->affirm=MagickFalse;  /* maybe SGI disguised as RGB */
    }
  /*
    Look for explicit 'format:image' in filename.
  */
  *magic='\0';
  GetPathComponent(image_info->filename,MagickPath,magic);
  if (*magic == '\0')
    {
      (void) CopyMagickString(magic,image_info->magick,MagickPathExtent);
      magick_info=GetMagickInfo(magic,sans_exception);
      if (frames == 0)
        GetPathComponent(image_info->filename,CanonicalPath,component);
      else
        GetPathComponent(image_info->filename,SubcanonicalPath,component);
      (void) CopyMagickString(image_info->filename,component,MagickPathExtent);
    }
  else
    {
      const DelegateInfo
        *delegate_info;

      /*
        User specified image format.
      */
      LocaleUpper(magic);
      magick_info=GetMagickInfo(magic,sans_exception);
      delegate_info=GetDelegateInfo(magic,"*",sans_exception);
      if (delegate_info == (const DelegateInfo *) NULL)
        delegate_info=GetDelegateInfo("*",magic,sans_exception);
      if (((magick_info != (const MagickInfo *) NULL) ||
           (delegate_info != (const DelegateInfo *) NULL)) &&
          (IsMagickConflict(magic) == MagickFalse))
        {
          image_info->affirm=MagickTrue;
          (void) CopyMagickString(image_info->magick,magic,MagickPathExtent);
          GetPathComponent(image_info->filename,CanonicalPath,component);
          (void) CopyMagickString(image_info->filename,component,
            MagickPathExtent);
        }
    }
  sans_exception=DestroyExceptionInfo(sans_exception);
  if ((magick_info == (const MagickInfo *) NULL) ||
      (GetMagickEndianSupport(magick_info) == MagickFalse))
    image_info->endian=UndefinedEndian;
  if ((image_info->adjoin != MagickFalse) && (frames > 1))
    {
      /*
        Test for multiple image support (e.g. image%02d.png).
      */
      (void) InterpretImageFilename(image_info,(Image *) NULL,
        image_info->filename,(int) image_info->scene,component,exception);
      if ((LocaleCompare(component,image_info->filename) != 0) &&
          (strchr(component,'%') == (char *) NULL))
        image_info->adjoin=MagickFalse;
    }
  if ((image_info->adjoin != MagickFalse) && (frames > 0))
    {
      /*
        Some image formats do not support multiple frames per file.
      */
      magick_info=GetMagickInfo(magic,exception);
      if (magick_info != (const MagickInfo *) NULL)
        if (GetMagickAdjoin(magick_info) == MagickFalse)
          image_info->adjoin=MagickFalse;
    }
  if (image_info->affirm != MagickFalse)
    return(MagickTrue);
  if (frames == 0)
    {
      unsigned char
        *magick;

      size_t
        magick_size;

      /*
        Determine the image format from the first few bytes of the file.
      */
      magick_size=GetMagicPatternExtent(exception);
      if (magick_size == 0)
        return(MagickFalse);
      image=AcquireImage(image_info,exception);
      (void) CopyMagickString(image->filename,image_info->filename,
        MagickPathExtent);
      status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
      if (status == MagickFalse)
        {
          image=DestroyImage(image);
          return(MagickFalse);
        }
      if ((IsBlobSeekable(image) == MagickFalse) ||
          (IsBlobExempt(image) != MagickFalse))
        {
          /*
            Copy image to seekable temporary file.
          */
          *component='\0';
          status=ImageToFile(image,component,exception);
          (void) CloseBlob(image);
          if (status == MagickFalse)
            {
              image=DestroyImage(image);
              return(MagickFalse);
            }
          SetImageInfoFile(image_info,(FILE *) NULL);
          (void) CopyMagickString(image->filename,component,MagickPathExtent);
          status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
          if (status == MagickFalse)
            {
              image=DestroyImage(image);
              return(MagickFalse);
            }
          (void) CopyMagickString(image_info->filename,component,
            MagickPathExtent);
          image_info->temporary=MagickTrue;
        }
      magick=(unsigned char *) AcquireMagickMemory(magick_size);
      if (magick == (unsigned char *) NULL)
        {
          (void) CloseBlob(image);
          image=DestroyImage(image);
          return(MagickFalse);
        }
      (void) memset(magick,0,magick_size);
      count=ReadBlob(image,magick_size,magick);
      (void) SeekBlob(image,-((MagickOffsetType) count),SEEK_CUR);
      (void) CloseBlob(image);
      image=DestroyImage(image);
      /*
        Check magic cache.
      */
      sans_exception=AcquireExceptionInfo();
      magic_info=GetMagicInfo(magick,(size_t) count,sans_exception);
      magick=(unsigned char *) RelinquishMagickMemory(magick);
      if ((magic_info != (const MagicInfo *) NULL) &&
          (GetMagicName(magic_info) != (char *) NULL))
        {
          /*
            Try to use magick_info that was determined earlier by the extension
          */
          if ((magick_info != (const MagickInfo *) NULL) &&
              (GetMagickUseExtension(magick_info) != MagickFalse) &&
              (LocaleCompare(magick_info->module,GetMagicName(
                magic_info)) == 0))
            (void) CopyMagickString(image_info->magick,magick_info->name,
              MagickPathExtent);
          else
            {
              (void) CopyMagickString(image_info->magick,GetMagicName(
                magic_info),MagickPathExtent);
              magick_info=GetMagickInfo(image_info->magick,sans_exception);
            }
          if ((magick_info == (const MagickInfo *) NULL) ||
              (GetMagickEndianSupport(magick_info) == MagickFalse))
            image_info->endian=UndefinedEndian;
          sans_exception=DestroyExceptionInfo(sans_exception);
          return(MagickTrue);
        }
      magick_info=GetMagickInfo(image_info->magick,sans_exception);
      if ((magick_info == (const MagickInfo *) NULL) ||
          (GetMagickEndianSupport(magick_info) == MagickFalse))
        image_info->endian=UndefinedEndian;
      sans_exception=DestroyExceptionInfo(sans_exception);
    }
  return(MagickTrue);
}