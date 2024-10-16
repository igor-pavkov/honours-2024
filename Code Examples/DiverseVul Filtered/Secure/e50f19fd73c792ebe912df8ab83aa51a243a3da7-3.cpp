static MagickBooleanType WriteXPMImage(const ImageInfo *image_info,Image *image,
  ExceptionInfo *exception)
{
#define MaxCixels  92

  static const char
    Cixel[MaxCixels+1] = " .XoO+@#$%&*=-;:>,<1234567890qwertyuipasdfghjk"
                         "lzxcvbnmMNBVCZASDFGHJKLPIUYTREWQ!~^/()_`'][{}|";

  char
    buffer[MagickPathExtent],
    basename[MagickPathExtent],
    name[MagickPathExtent],
    symbol[MagickPathExtent];

  MagickBooleanType
    status;

  PixelInfo
    pixel;

  const Quantum
    *p;

  ssize_t
    i,
    x;

  size_t
    characters_per_pixel;

  ssize_t
    j,
    k,
    opacity,
    y;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,exception);
  if (status == MagickFalse)
    return(status);
  if (IssRGBCompatibleColorspace(image->colorspace) == MagickFalse)
    if (IssRGBCompatibleColorspace(image->colorspace) == MagickFalse)
      (void) TransformImageColorspace(image,sRGBColorspace,exception);
  opacity=(-1);
  if (image->alpha_trait == UndefinedPixelTrait)
    {
      if ((image->storage_class == DirectClass) || (image->colors > 256))
        (void) SetImageType(image,PaletteType,exception);
    }
  else
    {
      double
        alpha,
        beta;

      /*
        Identify transparent colormap index.
      */
      if ((image->storage_class == DirectClass) || (image->colors > 256))
        (void) SetImageType(image,PaletteBilevelAlphaType,exception);
      for (i=0; i < (ssize_t) image->colors; i++)
        if (image->colormap[i].alpha != OpaqueAlpha)
          {
            if (opacity < 0)
              {
                opacity=i;
                continue;
              }
            alpha=(double) TransparentAlpha-(double)
              image->colormap[i].alpha;
            beta=(double) TransparentAlpha-(double)
              image->colormap[opacity].alpha;
            if (alpha < beta)
              opacity=i;
          }
      if (opacity == -1)
        {
          (void) SetImageType(image,PaletteBilevelAlphaType,exception);
          for (i=0; i < (ssize_t) image->colors; i++)
            if (image->colormap[i].alpha != OpaqueAlpha)
              {
                if (opacity < 0)
                  {
                    opacity=i;
                    continue;
                  }
                alpha=(Quantum) TransparentAlpha-(double)
                  image->colormap[i].alpha;
                beta=(Quantum) TransparentAlpha-(double)
                  image->colormap[opacity].alpha;
                if (alpha < beta)
                  opacity=i;
              }
        }
      if (opacity >= 0)
        {
          image->colormap[opacity].red=image->transparent_color.red;
          image->colormap[opacity].green=image->transparent_color.green;
          image->colormap[opacity].blue=image->transparent_color.blue;
        }
    }
  /*
    Compute the character per pixel.
  */
  characters_per_pixel=1;
  for (k=MaxCixels; (ssize_t) image->colors > k; k*=MaxCixels)
    characters_per_pixel++;
  /*
    XPM header.
  */
  (void) WriteBlobString(image,"/* XPM */\n");
  GetPathComponent(image->filename,BasePath,basename);
  if (isalnum((int) ((unsigned char) *basename)) == 0)
    {
      (void) FormatLocaleString(buffer,MagickPathExtent,"xpm_%.1024s",basename);
      (void) CopyMagickString(basename,buffer,MagickPathExtent);
    }
  if (isalpha((int) ((unsigned char) basename[0])) == 0)
    basename[0]='_';
  for (i=1; basename[i] != '\0'; i++)
    if (isalnum((int) ((unsigned char) basename[i])) == 0)
      basename[i]='_';
  (void) FormatLocaleString(buffer,MagickPathExtent,
    "static char *%.1024s[] = {\n",basename);
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,"/* columns rows colors chars-per-pixel */\n");
  (void) FormatLocaleString(buffer,MagickPathExtent,
    "\"%.20g %.20g %.20g %.20g \",\n",(double) image->columns,(double)
    image->rows,(double) image->colors,(double) characters_per_pixel);
  (void) WriteBlobString(image,buffer);
  GetPixelInfo(image,&pixel);
  for (i=0; i < (ssize_t) image->colors; i++)
  {
    const char
      *symbolic;

    /*
      Define XPM color.
    */
    pixel=image->colormap[i];
    pixel.colorspace=sRGBColorspace;
    pixel.depth=8;
    pixel.alpha=(double) OpaqueAlpha;
    (void) QueryColorname(image,&pixel,XPMCompliance,name,exception);
    if (i == opacity)
      (void) CopyMagickString(name,"None",MagickPathExtent);
    /*
      Write XPM color.
    */
    k=i % MaxCixels;
    symbol[0]=Cixel[k];
    for (j=1; j < (ssize_t) characters_per_pixel; j++)
    {
      k=((i-k)/MaxCixels) % MaxCixels;
      symbol[j]=Cixel[k];
    }
    symbol[j]='\0';
    symbolic=(const char *) GetValueFromSplayTree(xpm_symbolic,name);
    if (symbolic == (const char *) NULL)
      (void) FormatLocaleString(buffer,MagickPathExtent,
        "\"%.1024s c %.1024s\",\n",symbol,name);
    else
      (void) FormatLocaleString(buffer,MagickPathExtent,
        "\"%.1024s c %.1024s %.1024s\",\n",symbol,name,symbolic);
    (void) WriteBlobString(image,buffer);
  }
  /*
    Define XPM pixels.
  */
  (void) WriteBlobString(image,"/* pixels */\n");
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    p=GetVirtualPixels(image,0,y,image->columns,1,exception);
    if (p == (const Quantum *) NULL)
      break;
    (void) WriteBlobString(image,"\"");
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      k=((ssize_t) GetPixelIndex(image,p) % MaxCixels);
      symbol[0]=Cixel[k];
      for (j=1; j < (ssize_t) characters_per_pixel; j++)
      {
        k=(((int) GetPixelIndex(image,p)-k)/MaxCixels) % MaxCixels;
        symbol[j]=Cixel[k];
      }
      symbol[j]='\0';
      (void) CopyMagickString(buffer,symbol,MagickPathExtent);
      (void) WriteBlobString(image,buffer);
      p+=GetPixelChannels(image);
    }
    (void) FormatLocaleString(buffer,MagickPathExtent,"\"%.1024s\n",
      (y == (ssize_t) (image->rows-1) ? "" : ","));
    (void) WriteBlobString(image,buffer);
    if (image->previous == (Image *) NULL)
      {
        status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
          image->rows);
        if (status == MagickFalse)
          break;
      }
  }
  (void) WriteBlobString(image,"};\n");
  (void) CloseBlob(image);
  return(MagickTrue);
}