static void ReadPDFInfo(const ImageInfo *image_info,Image *image,
  PDFInfo *pdf_info,ExceptionInfo *exception)
{
#define CMYKProcessColor  "CMYKProcessColor"
#define CropBox  "CropBox"
#define DefaultCMYK  "DefaultCMYK"
#define DeviceCMYK  "DeviceCMYK"
#define MediaBox  "MediaBox"
#define PDFRotate  "Rotate"
#define SpotColor  "Separation"
#define TrimBox  "TrimBox"
#define PDFVersion  "PDF-"

  char
    version[MagickPathExtent];

  int
    c;

  MagickByteBuffer
    buffer;

  register char
    *p;

  register ssize_t
    i;

  SegmentInfo
    bounds;

  size_t
    spotcolor;

  ssize_t
    count;

  (void) memset(&bounds,0,sizeof(bounds));
  (void) memset(pdf_info,0,sizeof(*pdf_info));
  pdf_info->cmyk=image_info->colorspace == CMYKColorspace ? MagickTrue :
    MagickFalse;
  pdf_info->cropbox=IsStringTrue(GetImageOption(image_info,"pdf:use-cropbox"));
  pdf_info->trimbox=IsStringTrue(GetImageOption(image_info,"pdf:use-trimbox"));
  *version='\0';
  spotcolor=0;
  (void) memset(&buffer,0,sizeof(buffer));
  buffer.image=image;
  for (c=ReadMagickByteBuffer(&buffer); c != EOF; c=ReadMagickByteBuffer(&buffer))
  {
    switch(c)
    {
      case '%':
      {
        if (*version == '\0')
          {
            i=0;
            for (c=ReadMagickByteBuffer(&buffer); c != EOF; c=ReadMagickByteBuffer(&buffer))
            {
              if ((c == '\r') || (c == '\n') || ((i+1) == MagickPathExtent))
                break;
              version[i++]=(char) c;
            }
            version[i]='\0';
          }
        continue;
      }
      case '<':
      {
        ReadGhostScriptXMPProfile(&buffer,&pdf_info->profile);
        continue;
      }
      case '/':
        break;
      default:
        continue;
    }
    if (CompareMagickByteBuffer(&buffer,PDFRotate,strlen(PDFRotate)) != MagickFalse)
      {
        p=GetMagickByteBufferDatum(&buffer);
        (void) sscanf(p,PDFRotate" %lf",&pdf_info->angle);
      }
    if (pdf_info->cmyk == MagickFalse)
      {
        if ((CompareMagickByteBuffer(&buffer,DefaultCMYK,strlen(DefaultCMYK)) != MagickFalse) ||
            (CompareMagickByteBuffer(&buffer,DeviceCMYK,strlen(DeviceCMYK)) != MagickFalse) ||
            (CompareMagickByteBuffer(&buffer,CMYKProcessColor,strlen(CMYKProcessColor)) != MagickFalse))
          {
            pdf_info->cmyk=MagickTrue;
            continue;
          }
      }
    if (CompareMagickByteBuffer(&buffer,SpotColor,strlen(SpotColor)) != MagickFalse)
      {
        char
          name[MagickPathExtent],
          property[MagickPathExtent],
          *value;

        /*
          Note spot names.
        */
        (void) FormatLocaleString(property,MagickPathExtent,
          "pdf:SpotColor-%.20g",(double) spotcolor++);
        i=0;
        SkipMagickByteBuffer(&buffer,strlen(SpotColor)+1);
        for (c=ReadMagickByteBuffer(&buffer); c != EOF; c=ReadMagickByteBuffer(&buffer))
        {
          if ((isspace(c) != 0) || (c == '/') || ((i+1) == MagickPathExtent))
            break;
          name[i++]=(char) c;
        }
        name[i]='\0';
        value=ConstantString(name);
        (void) SubstituteString(&value,"#20"," ");
        if (*value != '\0')
          (void) SetImageProperty(image,property,value,exception);
        value=DestroyString(value);
        continue;
      }
    if (image_info->page != (char *) NULL)
      continue;
    count=0;
    if (pdf_info->cropbox != MagickFalse)
      {
        if (CompareMagickByteBuffer(&buffer,CropBox,strlen(CropBox)) != MagickFalse)
          {
            /*
              Note region defined by crop box.
            */
            p=GetMagickByteBufferDatum(&buffer);
            count=(ssize_t) sscanf(p,"CropBox [%lf %lf %lf %lf",&bounds.x1,
              &bounds.y1,&bounds.x2,&bounds.y2);
            if (count != 4)
              count=(ssize_t) sscanf(p,"CropBox[%lf %lf %lf %lf",&bounds.x1,
                &bounds.y1,&bounds.x2,&bounds.y2);
          }
      }
    else
      if (pdf_info->trimbox != MagickFalse)
        {
          if (CompareMagickByteBuffer(&buffer,TrimBox,strlen(TrimBox)) != MagickFalse)
            {
              /*
                Note region defined by trim box.
              */
              p=GetMagickByteBufferDatum(&buffer);
              count=(ssize_t) sscanf(p,"TrimBox [%lf %lf %lf %lf",&bounds.x1,
                &bounds.y1,&bounds.x2,&bounds.y2);
              if (count != 4)
                count=(ssize_t) sscanf(p,"TrimBox[%lf %lf %lf %lf",&bounds.x1,
                  &bounds.y1,&bounds.x2,&bounds.y2);
            }
        }
      else
        if (CompareMagickByteBuffer(&buffer,MediaBox,strlen(MediaBox)) != MagickFalse)
          {
            /*
              Note region defined by media box.
            */
            p=GetMagickByteBufferDatum(&buffer);
            count=(ssize_t) sscanf(p,"MediaBox [%lf %lf %lf %lf",&bounds.x1,
              &bounds.y1,&bounds.x2,&bounds.y2);
            if (count != 4)
              count=(ssize_t) sscanf(p,"MediaBox[%lf %lf %lf %lf",&bounds.x1,
                &bounds.y1,&bounds.x2,&bounds.y2);
          }
    if (count != 4)
      continue;
    if ((fabs(bounds.x2-bounds.x1) <= fabs(pdf_info->bounds.x2-pdf_info->bounds.x1)) ||
        (fabs(bounds.y2-bounds.y1) <= fabs(pdf_info->bounds.y2-pdf_info->bounds.y1)))
      continue;
    pdf_info->bounds=bounds;
  }
  if (version[0] != '\0')
    (void) SetImageProperty(image,"pdf:Version",version,exception);
}