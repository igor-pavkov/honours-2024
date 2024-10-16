static void MSLStartElement(void *context,const xmlChar *tag,
  const xmlChar **attributes)
{
  AffineMatrix
    affine,
    current;

  ChannelType
    channel;

  char
    *attribute,
    key[MaxTextExtent],
    *value;

  const char
    *keyword;

  double
    angle;

  DrawInfo
    *draw_info;

  ExceptionInfo
    *exception;

  GeometryInfo
    geometry_info;

  Image
    *image;

  int
    flags;

  ssize_t
    option,
    j,
    n,
    x,
    y;

  MSLInfo
    *msl_info;

  RectangleInfo
    geometry;

  register ssize_t
    i;

  size_t
    height,
    width;

  /*
    Called when an opening tag has been processed.
  */
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),
    "  SAX.startElement(%s",tag);
  exception=AcquireExceptionInfo();
  msl_info=(MSLInfo *) context;
  n=msl_info->n;
  keyword=(const char *) NULL;
  value=(char *) NULL;
  SetGeometryInfo(&geometry_info);
  (void) memset(&geometry,0,sizeof(geometry));
  channel=DefaultChannels;
  switch (*tag)
  {
    case 'A':
    case 'a':
    {
      if (LocaleCompare((const char *) tag,"add-noise") == 0)
        {
          Image
            *noise_image;

          NoiseType
            noise;

          /*
            Add noise image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          noise=UniformNoise;
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'C':
                case 'c':
                {
                  if (LocaleCompare(keyword,"channel") == 0)
                    {
                      option=ParseChannelOption(value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedChannelType",
                          value);
                      channel=(ChannelType) option;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'N':
                case 'n':
                {
                  if (LocaleCompare(keyword,"noise") == 0)
                    {
                      option=ParseCommandOption(MagickNoiseOptions,MagickFalse,
                        value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedNoiseType",
                          value);
                      noise=(NoiseType) option;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          noise_image=AddNoiseImageChannel(msl_info->image[n],channel,noise,
            &msl_info->image[n]->exception);
          if (noise_image == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=noise_image;
          break;
        }
      if (LocaleCompare((const char *) tag,"annotate") == 0)
        {
          char
            text[MaxTextExtent];

          /*
            Annotate image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          draw_info=CloneDrawInfo(msl_info->image_info[n],
            msl_info->draw_info[n]);
          angle=0.0;
          current=draw_info->affine;
          GetAffineMatrix(&affine);
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'A':
                case 'a':
                {
                  if (LocaleCompare(keyword,"affine") == 0)
                    {
                      char
                        *p;

                      p=value;
                      draw_info->affine.sx=StringToDouble(p,&p);
                      if (*p ==',')
                        p++;
                      draw_info->affine.rx=StringToDouble(p,&p);
                      if (*p ==',')
                        p++;
                      draw_info->affine.ry=StringToDouble(p,&p);
                      if (*p ==',')
                        p++;
                      draw_info->affine.sy=StringToDouble(p,&p);
                      if (*p ==',')
                        p++;
                      draw_info->affine.tx=StringToDouble(p,&p);
                      if (*p ==',')
                        p++;
                      draw_info->affine.ty=StringToDouble(p,&p);
                      break;
                    }
                  if (LocaleCompare(keyword,"align") == 0)
                    {
                      option=ParseCommandOption(MagickAlignOptions,MagickFalse,
                        value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedAlignType",
                          value);
                      draw_info->align=(AlignType) option;
                      break;
                    }
                  if (LocaleCompare(keyword,"antialias") == 0)
                    {
                      option=ParseCommandOption(MagickBooleanOptions,
                        MagickFalse,value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedBooleanType",
                          value);
                      draw_info->stroke_antialias=(MagickBooleanType) option;
                      draw_info->text_antialias=(MagickBooleanType) option;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'D':
                case 'd':
                {
                  if (LocaleCompare(keyword,"density") == 0)
                    {
                      CloneString(&draw_info->density,value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'E':
                case 'e':
                {
                  if (LocaleCompare(keyword,"encoding") == 0)
                    {
                      CloneString(&draw_info->encoding,value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'F':
                case 'f':
                {
                  if (LocaleCompare(keyword, "fill") == 0)
                    {
                      (void) QueryColorDatabase(value,&draw_info->fill,
                        exception);
                      break;
                    }
                  if (LocaleCompare(keyword,"family") == 0)
                    {
                      CloneString(&draw_info->family,value);
                      break;
                    }
                  if (LocaleCompare(keyword,"font") == 0)
                    {
                      CloneString(&draw_info->font,value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"geometry") == 0)
                    {
                      flags=ParsePageGeometry(msl_info->image[n],value,
                        &geometry,exception);
                      if ((flags & HeightValue) == 0)
                        geometry.height=geometry.width;
                      break;
                    }
                  if (LocaleCompare(keyword,"gravity") == 0)
                    {
                      option=ParseCommandOption(MagickGravityOptions,
                        MagickFalse,value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedGravityType",
                          value);
                      draw_info->gravity=(GravityType) option;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'P':
                case 'p':
                {
                  if (LocaleCompare(keyword,"pointsize") == 0)
                    {
                      draw_info->pointsize=StringToDouble(value,(char **) NULL);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'R':
                case 'r':
                {
                  if (LocaleCompare(keyword,"rotate") == 0)
                    {
                      angle=StringToDouble(value,(char **) NULL);
                      affine.sx=cos(DegreesToRadians(fmod(angle,360.0)));
                      affine.rx=sin(DegreesToRadians(fmod(angle,360.0)));
                      affine.ry=(-sin(DegreesToRadians(fmod(angle,360.0))));
                      affine.sy=cos(DegreesToRadians(fmod(angle,360.0)));
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'S':
                case 's':
                {
                  if (LocaleCompare(keyword,"scale") == 0)
                    {
                      flags=ParseGeometry(value,&geometry_info);
                      if ((flags & SigmaValue) == 0)
                        geometry_info.sigma=1.0;
                      affine.sx=geometry_info.rho;
                      affine.sy=geometry_info.sigma;
                      break;
                    }
                  if (LocaleCompare(keyword,"skewX") == 0)
                    {
                      angle=StringToDouble(value,(char **) NULL);
                      affine.ry=tan(DegreesToRadians(fmod((double) angle,
                        360.0)));
                      break;
                    }
                  if (LocaleCompare(keyword,"skewY") == 0)
                    {
                      angle=StringToDouble(value,(char **) NULL);
                      affine.rx=tan(DegreesToRadians(fmod((double) angle,
                        360.0)));
                      break;
                    }
                  if (LocaleCompare(keyword,"stretch") == 0)
                    {
                      option=ParseCommandOption(MagickStretchOptions,
                        MagickFalse,value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedStretchType",
                          value);
                      draw_info->stretch=(StretchType) option;
                      break;
                    }
                  if (LocaleCompare(keyword, "stroke") == 0)
                    {
                      (void) QueryColorDatabase(value,&draw_info->stroke,
                        exception);
                      break;
                    }
                  if (LocaleCompare(keyword,"strokewidth") == 0)
                    {
                      draw_info->stroke_width=StringToLong(value);
                      break;
                    }
                  if (LocaleCompare(keyword,"style") == 0)
                    {
                      option=ParseCommandOption(MagickStyleOptions,MagickFalse,
                        value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedStyleType",
                          value);
                      draw_info->style=(StyleType) option;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'T':
                case 't':
                {
                  if (LocaleCompare(keyword,"text") == 0)
                    {
                      CloneString(&draw_info->text,value);
                      break;
                    }
                  if (LocaleCompare(keyword,"translate") == 0)
                    {
                      flags=ParseGeometry(value,&geometry_info);
                      if ((flags & SigmaValue) == 0)
                        geometry_info.sigma=1.0;
                      affine.tx=geometry_info.rho;
                      affine.ty=geometry_info.sigma;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'U':
                case 'u':
                {
                  if (LocaleCompare(keyword, "undercolor") == 0)
                    {
                      (void) QueryColorDatabase(value,&draw_info->undercolor,
                        exception);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'W':
                case 'w':
                {
                  if (LocaleCompare(keyword,"weight") == 0)
                    {
                      draw_info->weight=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'X':
                case 'x':
                {
                  if (LocaleCompare(keyword,"x") == 0)
                    {
                      geometry.x=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'Y':
                case 'y':
                {
                  if (LocaleCompare(keyword,"y") == 0)
                    {
                      geometry.y=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          (void) FormatLocaleString(text,MaxTextExtent,
            "%.20gx%.20g%+.20g%+.20g",(double) geometry.width,(double)
            geometry.height,(double) geometry.x,(double) geometry.y);
          CloneString(&draw_info->geometry,text);
          draw_info->affine.sx=affine.sx*current.sx+affine.ry*current.rx;
          draw_info->affine.rx=affine.rx*current.sx+affine.sy*current.rx;
          draw_info->affine.ry=affine.sx*current.ry+affine.ry*current.sy;
          draw_info->affine.sy=affine.rx*current.ry+affine.sy*current.sy;
          draw_info->affine.tx=affine.sx*current.tx+affine.ry*current.ty+
            affine.tx;
          draw_info->affine.ty=affine.rx*current.tx+affine.sy*current.ty+
            affine.ty;
          (void) AnnotateImage(msl_info->image[n],draw_info);
          draw_info=DestroyDrawInfo(draw_info);
          break;
        }
      if (LocaleCompare((const char *) tag,"append") == 0)
        {
          Image
            *append_image;

          MagickBooleanType
            stack;

          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          stack=MagickFalse;
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'S':
                case 's':
                {
                  if (LocaleCompare(keyword,"stack") == 0)
                    {
                      option=ParseCommandOption(MagickBooleanOptions,MagickFalse,
                        value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedBooleanType",
                          value);
                      stack=(MagickBooleanType) option;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          append_image=AppendImages(msl_info->image[n],stack,
            &msl_info->image[n]->exception);
          if (append_image == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=append_image;
          break;
        }
      ThrowMSLException(OptionError,"UnrecognizedElement",(const char *) tag);
      break;
    }
    case 'B':
    case 'b':
    {
      if (LocaleCompare((const char *) tag,"blur") == 0)
        {
          Image
            *blur_image;

          /*
            Blur image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'C':
                case 'c':
                {
                  if (LocaleCompare(keyword,"channel") == 0)
                    {
                      option=ParseChannelOption(value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedChannelType",
                          value);
                      channel=(ChannelType) option;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"geometry") == 0)
                    {
                      flags=ParseGeometry(value,&geometry_info);
                      if ((flags & SigmaValue) == 0)
                        geometry_info.sigma=1.0;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'R':
                case 'r':
                {
                  if (LocaleCompare(keyword,"radius") == 0)
                    {
                      geometry_info.rho=StringToDouble(value,(char **) NULL);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'S':
                case 's':
                {
                  if (LocaleCompare(keyword,"sigma") == 0)
                    {
                      geometry_info.sigma=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          blur_image=BlurImageChannel(msl_info->image[n],channel,
            geometry_info.rho,geometry_info.sigma,
            &msl_info->image[n]->exception);
          if (blur_image == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=blur_image;
          break;
        }
      if (LocaleCompare((const char *) tag,"border") == 0)
        {
          Image
            *border_image;

          /*
            Border image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          SetGeometry(msl_info->image[n],&geometry);
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'C':
                case 'c':
                {
                  if (LocaleCompare(keyword,"compose") == 0)
                    {
                      option=ParseCommandOption(MagickComposeOptions,MagickFalse,
                        value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedComposeType",
                          value);
                      msl_info->image[n]->compose=(CompositeOperator) option;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'F':
                case 'f':
                {
                  if (LocaleCompare(keyword, "fill") == 0)
                    {
                      (void) QueryColorDatabase(value,
                        &msl_info->image[n]->border_color,exception);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"geometry") == 0)
                    {
                      flags=ParsePageGeometry(msl_info->image[n],value,
                        &geometry,exception);
                      if ((flags & HeightValue) == 0)
                        geometry.height=geometry.width;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'H':
                case 'h':
                {
                  if (LocaleCompare(keyword,"height") == 0)
                    {
                      geometry.height=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'W':
                case 'w':
                {
                  if (LocaleCompare(keyword,"width") == 0)
                    {
                      geometry.width=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          border_image=BorderImage(msl_info->image[n],&geometry,
            &msl_info->image[n]->exception);
          if (border_image == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=border_image;
          break;
        }
      ThrowMSLException(OptionError,"UnrecognizedElement",(const char *) tag);
    }
    case 'C':
    case 'c':
    {
      if (LocaleCompare((const char *) tag,"colorize") == 0)
        {
          char
            opacity[MaxTextExtent];

          Image
            *colorize_image;

          PixelPacket
            target;

          /*
            Add noise image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          target=msl_info->image[n]->background_color;
          (void) CopyMagickString(opacity,"100",MaxTextExtent);
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'F':
                case 'f':
                {
                  if (LocaleCompare(keyword,"fill") == 0)
                    {
                      (void) QueryColorDatabase(value,&target,
                        &msl_info->image[n]->exception);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'O':
                case 'o':
                {
                  if (LocaleCompare(keyword,"opacity") == 0)
                    {
                      (void) CopyMagickString(opacity,value,MaxTextExtent);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          colorize_image=ColorizeImage(msl_info->image[n],opacity,target,
            &msl_info->image[n]->exception);
          if (colorize_image == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=colorize_image;
          break;
        }
      if (LocaleCompare((const char *) tag, "charcoal") == 0)
      {
        double  radius = 0.0,
            sigma = 1.0;

        if (msl_info->image[n] == (Image *) NULL)
        {
          ThrowMSLException(OptionError,"NoImagesDefined",
            (const char *) tag);
          break;
        }
        /*
        NOTE: charcoal can have no attributes, since we use all the defaults!
        */
        if (attributes != (const xmlChar **) NULL)
        {
          for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
          {
            keyword=(const char *) attributes[i++];
            attribute=InterpretImageProperties(msl_info->image_info[n],
              msl_info->attributes[n],(const char *) attributes[i]);
            CloneString(&value,attribute);
            attribute=DestroyString(attribute);
          switch (*keyword)
          {
            case 'R':
            case 'r':
            {
              if (LocaleCompare(keyword,"radius") == 0)
                {
                  radius=StringToDouble(value,(char **) NULL);
                  break;
                }
              ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
              break;
            }
            case 'S':
            case 's':
            {
              if (LocaleCompare(keyword,"sigma") == 0)
              {
                sigma = StringToLong( value );
                break;
              }
              ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
              break;
            }
            default:
            {
              ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
              break;
            }
          }
          }
        }

        /*
          charcoal image.
        */
        {
        Image
          *newImage;

        newImage=CharcoalImage(msl_info->image[n],radius,sigma,
          &msl_info->image[n]->exception);
        if (newImage == (Image *) NULL)
          break;
        msl_info->image[n]=DestroyImage(msl_info->image[n]);
        msl_info->image[n]=newImage;
        break;
        }
      }
      if (LocaleCompare((const char *) tag,"chop") == 0)
        {
          Image
            *chop_image;

          /*
            Chop image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          SetGeometry(msl_info->image[n],&geometry);
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"geometry") == 0)
                    {
                      flags=ParsePageGeometry(msl_info->image[n],value,
                        &geometry,exception);
                      if ((flags & HeightValue) == 0)
                        geometry.height=geometry.width;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'H':
                case 'h':
                {
                  if (LocaleCompare(keyword,"height") == 0)
                    {
                      geometry.height=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'W':
                case 'w':
                {
                  if (LocaleCompare(keyword,"width") == 0)
                    {
                      geometry.width=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'X':
                case 'x':
                {
                  if (LocaleCompare(keyword,"x") == 0)
                    {
                      geometry.x=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'Y':
                case 'y':
                {
                  if (LocaleCompare(keyword,"y") == 0)
                    {
                      geometry.y=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          chop_image=ChopImage(msl_info->image[n],&geometry,
            &msl_info->image[n]->exception);
          if (chop_image == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=chop_image;
          break;
        }
      if (LocaleCompare((const char *) tag,"color-floodfill") == 0)
        {
          PaintMethod
            paint_method;

          MagickPixelPacket
            target;

          /*
            Color floodfill image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          draw_info=CloneDrawInfo(msl_info->image_info[n],
            msl_info->draw_info[n]);
          SetGeometry(msl_info->image[n],&geometry);
          paint_method=FloodfillMethod;
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'B':
                case 'b':
                {
                  if (LocaleCompare(keyword,"bordercolor") == 0)
                    {
                      (void) QueryMagickColor(value,&target,exception);
                      paint_method=FillToBorderMethod;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'F':
                case 'f':
                {
                  if (LocaleCompare(keyword,"fill") == 0)
                    {
                      (void) QueryColorDatabase(value,&draw_info->fill,
                        exception);
                      break;
                    }
                  if (LocaleCompare(keyword,"fuzz") == 0)
                    {
                      msl_info->image[n]->fuzz=StringToDouble(value,
                        (char **) NULL);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"geometry") == 0)
                    {
                      flags=ParsePageGeometry(msl_info->image[n],value,
                        &geometry,exception);
                      if ((flags & HeightValue) == 0)
                        geometry.height=geometry.width;
                      (void) GetOneVirtualMagickPixel(msl_info->image[n],
                        geometry.x,geometry.y,&target,exception);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'X':
                case 'x':
                {
                  if (LocaleCompare(keyword,"x") == 0)
                    {
                      geometry.x=StringToLong(value);
                      (void) GetOneVirtualMagickPixel(msl_info->image[n],
                        geometry.x,geometry.y,&target,exception);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'Y':
                case 'y':
                {
                  if (LocaleCompare(keyword,"y") == 0)
                    {
                      geometry.y=StringToLong(value);
                      (void) GetOneVirtualMagickPixel(msl_info->image[n],
                        geometry.x,geometry.y,&target,exception);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          (void) FloodfillPaintImage(msl_info->image[n],DefaultChannels,
            draw_info,&target,geometry.x,geometry.y,
            paint_method == FloodfillMethod ? MagickFalse : MagickTrue);
          draw_info=DestroyDrawInfo(draw_info);
          break;
        }
      if (LocaleCompare((const char *) tag,"comment") == 0)
        break;
      if (LocaleCompare((const char *) tag,"composite") == 0)
        {
          char
            composite_geometry[MaxTextExtent];

          CompositeOperator
            compose;

          Image
            *composite_image,
            *rotate_image;

          PixelPacket
            target;

          /*
            Composite image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          composite_image=NewImageList();
          compose=OverCompositeOp;
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'C':
                case 'c':
                {
                  if (LocaleCompare(keyword,"compose") == 0)
                    {
                      option=ParseCommandOption(MagickComposeOptions,MagickFalse,
                        value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedComposeType",
                          value);
                      compose=(CompositeOperator) option;
                      break;
                    }
                  break;
                }
                case 'I':
                case 'i':
                {
                  if (LocaleCompare(keyword,"image") == 0)
                    for (j=0; j < msl_info->n; j++)
                    {
                      const char
                        *attribute;

                      attribute=GetImageProperty(msl_info->attributes[j],"id");
                      if ((attribute != (const char *) NULL)  &&
                          (LocaleCompare(attribute,value) == 0))
                        {
                          composite_image=CloneImage(msl_info->image[j],0,0,
                            MagickFalse,exception);
                          break;
                        }
                    }
                  break;
                }
                default:
                  break;
              }
            }
          if (composite_image == (Image *) NULL)
            break;
          rotate_image=NewImageList();
          SetGeometry(msl_info->image[n],&geometry);
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'B':
                case 'b':
                {
                  if (LocaleCompare(keyword,"blend") == 0)
                    {
                      (void) SetImageArtifact(composite_image,
                                            "compose:args",value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'C':
                case 'c':
                {
                  if (LocaleCompare(keyword,"channel") == 0)
                    {
                      option=ParseChannelOption(value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedChannelType",
                          value);
                      channel=(ChannelType) option;
                      break;
                    }
                  if (LocaleCompare(keyword, "color") == 0)
                    {
                      (void) QueryColorDatabase(value,
                        &composite_image->background_color,exception);
                      break;
                    }
                  if (LocaleCompare(keyword,"compose") == 0)
                    break;
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"geometry") == 0)
                    {
                      flags=ParsePageGeometry(msl_info->image[n],value,
                        &geometry,exception);
                      if ((flags & HeightValue) == 0)
                        geometry.height=geometry.width;
                      (void) GetOneVirtualPixel(msl_info->image[n],geometry.x,
                        geometry.y,&target,exception);
                      break;
                    }
                  if (LocaleCompare(keyword,"gravity") == 0)
                    {
                      option=ParseCommandOption(MagickGravityOptions,MagickFalse,
                        value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedGravityType",
                          value);
                      msl_info->image[n]->gravity=(GravityType) option;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'I':
                case 'i':
                {
                  if (LocaleCompare(keyword,"image") == 0)
                    break;
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'M':
                case 'm':
                {
                  if (LocaleCompare(keyword,"mask") == 0)
                    for (j=0; j < msl_info->n; j++)
                    {
                      const char
                        *attribute;

                      attribute=GetImageProperty(msl_info->attributes[j],"id");
                      if ((attribute != (const char *) NULL)  &&
                          (LocaleCompare(value,value) == 0))
                        {
                          SetImageType(composite_image,TrueColorMatteType);
                          (void) CompositeImage(composite_image,
                            CopyOpacityCompositeOp,msl_info->image[j],0,0);
                          break;
                        }
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'O':
                case 'o':
                {
                  if (LocaleCompare(keyword,"opacity") == 0)
                    {
                      ssize_t
                        opacity,
                        y;

                      register ssize_t
                        x;

                      register PixelPacket
                        *q;

                      CacheView
                        *composite_view;

                      opacity=QuantumRange-StringToLong(value);
                      if (compose != DissolveCompositeOp)
                        {
                          (void) SetImageOpacity(composite_image,(Quantum)
                            opacity);
                          break;
                        }
                      (void) SetImageArtifact(msl_info->image[n],
                                            "compose:args",value);
                      if (composite_image->matte != MagickTrue)
                        (void) SetImageOpacity(composite_image,OpaqueOpacity);
                      composite_view=AcquireAuthenticCacheView(composite_image,
                        exception);
                      for (y=0; y < (ssize_t) composite_image->rows ; y++)
                      {
                        q=GetCacheViewAuthenticPixels(composite_view,0,y,
                          (ssize_t) composite_image->columns,1,exception);
                        for (x=0; x < (ssize_t) composite_image->columns; x++)
                        {
                          if (q->opacity == OpaqueOpacity)
                            q->opacity=ClampToQuantum(opacity);
                          q++;
                        }
                        if (SyncCacheViewAuthenticPixels(composite_view,exception) == MagickFalse)
                          break;
                      }
                      composite_view=DestroyCacheView(composite_view);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'R':
                case 'r':
                {
                  if (LocaleCompare(keyword,"rotate") == 0)
                    {
                      rotate_image=RotateImage(composite_image,
                        StringToDouble(value,(char **) NULL),exception);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'T':
                case 't':
                {
                  if (LocaleCompare(keyword,"tile") == 0)
                    {
                      MagickBooleanType
                        tile;

                      option=ParseCommandOption(MagickBooleanOptions,MagickFalse,
                        value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedBooleanType",
                          value);
                      tile=(MagickBooleanType) option;
                      (void) tile;
                      if (rotate_image != (Image *) NULL)
                        (void) SetImageArtifact(rotate_image,
                          "compose:outside-overlay","false");
                      else
                        (void) SetImageArtifact(composite_image,
                          "compose:outside-overlay","false");
                       image=msl_info->image[n];
                       height=composite_image->rows;
                       width=composite_image->columns;
                       for (y=0; y < (ssize_t) image->rows; y+=(ssize_t) height)
                         for (x=0; x < (ssize_t) image->columns; x+=(ssize_t) width)
                         {
                           if (rotate_image != (Image *) NULL)
                             (void) CompositeImage(image,compose,rotate_image,
                               x,y);
                           else
                             (void) CompositeImage(image,compose,
                               composite_image,x,y);
                         }
                      if (rotate_image != (Image *) NULL)
                        rotate_image=DestroyImage(rotate_image);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'X':
                case 'x':
                {
                  if (LocaleCompare(keyword,"x") == 0)
                    {
                      geometry.x=StringToLong(value);
                      (void) GetOneVirtualPixel(msl_info->image[n],geometry.x,
                        geometry.y,&target,exception);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'Y':
                case 'y':
                {
                  if (LocaleCompare(keyword,"y") == 0)
                    {
                      geometry.y=StringToLong(value);
                      (void) GetOneVirtualPixel(msl_info->image[n],geometry.x,
                        geometry.y,&target,exception);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          image=msl_info->image[n];
          (void) FormatLocaleString(composite_geometry,MaxTextExtent,
            "%.20gx%.20g%+.20g%+.20g",(double) composite_image->columns,
            (double) composite_image->rows,(double) geometry.x,(double)
            geometry.y);
          flags=ParseGravityGeometry(image,composite_geometry,&geometry,
            exception);
          if (rotate_image == (Image *) NULL)
            CompositeImageChannel(image,channel,compose,composite_image,
              geometry.x,geometry.y);
          else
            {
              /*
                Rotate image.
              */
              geometry.x-=(ssize_t) (rotate_image->columns-
                composite_image->columns)/2;
              geometry.y-=(ssize_t) (rotate_image->rows-composite_image->rows)/2;
              CompositeImageChannel(image,channel,compose,rotate_image,
                geometry.x,geometry.y);
              rotate_image=DestroyImage(rotate_image);
            }
          composite_image=DestroyImage(composite_image);
          break;
        }
      if (LocaleCompare((const char *) tag,"contrast") == 0)
        {
          MagickBooleanType
            sharpen;

          /*
            Contrast image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          sharpen=MagickFalse;
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'S':
                case 's':
                {
                  if (LocaleCompare(keyword,"sharpen") == 0)
                    {
                      option=ParseCommandOption(MagickBooleanOptions,MagickFalse,
                        value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedBooleanType",
                          value);
                      sharpen=(MagickBooleanType) option;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          (void) ContrastImage(msl_info->image[n],sharpen);
          break;
        }
      if (LocaleCompare((const char *) tag,"crop") == 0)
        {
          Image
            *crop_image;

          /*
            Crop image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          SetGeometry(msl_info->image[n],&geometry);
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"geometry") == 0)
                    {
                      flags=ParseGravityGeometry(msl_info->image[n],value,
                        &geometry,exception);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'H':
                case 'h':
                {
                  if (LocaleCompare(keyword,"height") == 0)
                    {
                      geometry.height=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'W':
                case 'w':
                {
                  if (LocaleCompare(keyword,"width") == 0)
                    {
                      geometry.width=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'X':
                case 'x':
                {
                  if (LocaleCompare(keyword,"x") == 0)
                    {
                      geometry.x=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'Y':
                case 'y':
                {
                  if (LocaleCompare(keyword,"y") == 0)
                    {
                      geometry.y=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          crop_image=CropImage(msl_info->image[n],&geometry,
            &msl_info->image[n]->exception);
          if (crop_image == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=crop_image;
          break;
        }
      if (LocaleCompare((const char *) tag,"cycle-colormap") == 0)
        {
          ssize_t
            display;

          /*
            Cycle-colormap image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          display=0;
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'D':
                case 'd':
                {
                  if (LocaleCompare(keyword,"display") == 0)
                    {
                      display=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          (void) CycleColormapImage(msl_info->image[n],display);
          break;
        }
      ThrowMSLException(OptionError,"UnrecognizedElement",(const char *) tag);
    }
    case 'D':
    case 'd':
    {
      if (LocaleCompare((const char *) tag,"despeckle") == 0)
        {
          Image
            *despeckle_image;

          /*
            Despeckle image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
            }
          despeckle_image=DespeckleImage(msl_info->image[n],
            &msl_info->image[n]->exception);
          if (despeckle_image == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=despeckle_image;
          break;
        }
      if (LocaleCompare((const char *) tag,"display") == 0)
        {
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          (void) DisplayImages(msl_info->image_info[n],msl_info->image[n]);
          break;
        }
      if (LocaleCompare((const char *) tag,"draw") == 0)
        {
          char
            text[MaxTextExtent];

          /*
            Annotate image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          draw_info=CloneDrawInfo(msl_info->image_info[n],
            msl_info->draw_info[n]);
          angle=0.0;
          current=draw_info->affine;
          GetAffineMatrix(&affine);
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'A':
                case 'a':
                {
                  if (LocaleCompare(keyword,"affine") == 0)
                    {
                      char
                        *p;

                      p=value;
                      draw_info->affine.sx=StringToDouble(p,&p);
                      if (*p ==',')
                        p++;
                      draw_info->affine.rx=StringToDouble(p,&p);
                      if (*p ==',')
                        p++;
                      draw_info->affine.ry=StringToDouble(p,&p);
                      if (*p ==',')
                        p++;
                      draw_info->affine.sy=StringToDouble(p,&p);
                      if (*p ==',')
                        p++;
                      draw_info->affine.tx=StringToDouble(p,&p);
                      if (*p ==',')
                        p++;
                      draw_info->affine.ty=StringToDouble(p,&p);
                      break;
                    }
                  if (LocaleCompare(keyword,"align") == 0)
                    {
                      option=ParseCommandOption(MagickAlignOptions,MagickFalse,
                        value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedAlignType",
                          value);
                      draw_info->align=(AlignType) option;
                      break;
                    }
                  if (LocaleCompare(keyword,"antialias") == 0)
                    {
                      option=ParseCommandOption(MagickBooleanOptions,MagickFalse,
                        value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedBooleanType",
                          value);
                      draw_info->stroke_antialias=(MagickBooleanType) option;
                      draw_info->text_antialias=(MagickBooleanType) option;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'D':
                case 'd':
                {
                  if (LocaleCompare(keyword,"density") == 0)
                    {
                      CloneString(&draw_info->density,value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'E':
                case 'e':
                {
                  if (LocaleCompare(keyword,"encoding") == 0)
                    {
                      CloneString(&draw_info->encoding,value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'F':
                case 'f':
                {
                  if (LocaleCompare(keyword, "fill") == 0)
                    {
                      (void) QueryColorDatabase(value,&draw_info->fill,
                        exception);
                      break;
                    }
                  if (LocaleCompare(keyword,"family") == 0)
                    {
                      CloneString(&draw_info->family,value);
                      break;
                    }
                  if (LocaleCompare(keyword,"font") == 0)
                    {
                      CloneString(&draw_info->font,value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"geometry") == 0)
                    {
                      flags=ParsePageGeometry(msl_info->image[n],value,
                        &geometry,exception);
                      if ((flags & HeightValue) == 0)
                        geometry.height=geometry.width;
                      break;
                    }
                  if (LocaleCompare(keyword,"gravity") == 0)
                    {
                      option=ParseCommandOption(MagickGravityOptions,MagickFalse,
                        value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedGravityType",
                          value);
                      draw_info->gravity=(GravityType) option;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'P':
                case 'p':
                {
                  if (LocaleCompare(keyword,"points") == 0)
                    {
                      if (LocaleCompare(draw_info->primitive,"path") == 0)
                        {
                          (void) ConcatenateString(&draw_info->primitive," '");
                          ConcatenateString(&draw_info->primitive,value);
                          (void) ConcatenateString(&draw_info->primitive,"'");
                        }
                      else
                        {
                          (void) ConcatenateString(&draw_info->primitive," ");
                          ConcatenateString(&draw_info->primitive,value);
                        }
                      break;
                    }
                  if (LocaleCompare(keyword,"pointsize") == 0)
                    {
                      draw_info->pointsize=StringToDouble(value,
                        (char **) NULL);
                      break;
                    }
                  if (LocaleCompare(keyword,"primitive") == 0)
                    {
                      CloneString(&draw_info->primitive,value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'R':
                case 'r':
                {
                  if (LocaleCompare(keyword,"rotate") == 0)
                    {
                      angle=StringToDouble(value,(char **) NULL);
                      affine.sx=cos(DegreesToRadians(fmod(angle,360.0)));
                      affine.rx=sin(DegreesToRadians(fmod(angle,360.0)));
                      affine.ry=(-sin(DegreesToRadians(fmod(angle,360.0))));
                      affine.sy=cos(DegreesToRadians(fmod(angle,360.0)));
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'S':
                case 's':
                {
                  if (LocaleCompare(keyword,"scale") == 0)
                    {
                      flags=ParseGeometry(value,&geometry_info);
                      if ((flags & SigmaValue) == 0)
                        geometry_info.sigma=1.0;
                      affine.sx=geometry_info.rho;
                      affine.sy=geometry_info.sigma;
                      break;
                    }
                  if (LocaleCompare(keyword,"skewX") == 0)
                    {
                      angle=StringToDouble(value,(char **) NULL);
                      affine.ry=cos(DegreesToRadians(fmod(angle,360.0)));
                      break;
                    }
                  if (LocaleCompare(keyword,"skewY") == 0)
                    {
                      angle=StringToDouble(value,(char **) NULL);
                      affine.rx=cos(DegreesToRadians(fmod(angle,360.0)));
                      break;
                    }
                  if (LocaleCompare(keyword,"stretch") == 0)
                    {
                      option=ParseCommandOption(MagickStretchOptions,MagickFalse,
                        value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedStretchType",
                          value);
                      draw_info->stretch=(StretchType) option;
                      break;
                    }
                  if (LocaleCompare(keyword, "stroke") == 0)
                    {
                      (void) QueryColorDatabase(value,&draw_info->stroke,
                        exception);
                      break;
                    }
                  if (LocaleCompare(keyword,"strokewidth") == 0)
                    {
                      draw_info->stroke_width=StringToLong(value);
                      break;
                    }
                  if (LocaleCompare(keyword,"style") == 0)
                    {
                      option=ParseCommandOption(MagickStyleOptions,MagickFalse,
                        value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedStyleType",
                          value);
                      draw_info->style=(StyleType) option;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'T':
                case 't':
                {
                  if (LocaleCompare(keyword,"text") == 0)
                    {
                      (void) ConcatenateString(&draw_info->primitive," '");
                      (void) ConcatenateString(&draw_info->primitive,value);
                      (void) ConcatenateString(&draw_info->primitive,"'");
                      break;
                    }
                  if (LocaleCompare(keyword,"translate") == 0)
                    {
                      flags=ParseGeometry(value,&geometry_info);
                      if ((flags & SigmaValue) == 0)
                        geometry_info.sigma=1.0;
                      affine.tx=geometry_info.rho;
                      affine.ty=geometry_info.sigma;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'U':
                case 'u':
                {
                  if (LocaleCompare(keyword, "undercolor") == 0)
                    {
                      (void) QueryColorDatabase(value,&draw_info->undercolor,
                        exception);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'W':
                case 'w':
                {
                  if (LocaleCompare(keyword,"weight") == 0)
                    {
                      draw_info->weight=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'X':
                case 'x':
                {
                  if (LocaleCompare(keyword,"x") == 0)
                    {
                      geometry.x=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'Y':
                case 'y':
                {
                  if (LocaleCompare(keyword,"y") == 0)
                    {
                      geometry.y=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          (void) FormatLocaleString(text,MaxTextExtent,
            "%.20gx%.20g%+.20g%+.20g",(double) geometry.width,(double)
            geometry.height,(double) geometry.x,(double) geometry.y);
          CloneString(&draw_info->geometry,text);
          draw_info->affine.sx=affine.sx*current.sx+affine.ry*current.rx;
          draw_info->affine.rx=affine.rx*current.sx+affine.sy*current.rx;
          draw_info->affine.ry=affine.sx*current.ry+affine.ry*current.sy;
          draw_info->affine.sy=affine.rx*current.ry+affine.sy*current.sy;
          draw_info->affine.tx=affine.sx*current.tx+affine.ry*current.ty+
            affine.tx;
          draw_info->affine.ty=affine.rx*current.tx+affine.sy*current.ty+
            affine.ty;
          (void) DrawImage(msl_info->image[n],draw_info);
          draw_info=DestroyDrawInfo(draw_info);
          break;
        }
      ThrowMSLException(OptionError,"UnrecognizedElement",(const char *) tag);
    }
    case 'E':
    case 'e':
    {
      if (LocaleCompare((const char *) tag,"edge") == 0)
        {
          Image
            *edge_image;

          /*
            Edge image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"geometry") == 0)
                    {
                      flags=ParseGeometry(value,&geometry_info);
                      if ((flags & SigmaValue) == 0)
                        geometry_info.sigma=1.0;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'R':
                case 'r':
                {
                  if (LocaleCompare(keyword,"radius") == 0)
                    {
                      geometry_info.rho=StringToDouble(value,
                        (char **) NULL);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          edge_image=EdgeImage(msl_info->image[n],geometry_info.rho,
            &msl_info->image[n]->exception);
          if (edge_image == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=edge_image;
          break;
        }
      if (LocaleCompare((const char *) tag,"emboss") == 0)
        {
          Image
            *emboss_image;

          /*
            Emboss image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"geometry") == 0)
                    {
                      flags=ParseGeometry(value,&geometry_info);
                      if ((flags & SigmaValue) == 0)
                        geometry_info.sigma=1.0;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'R':
                case 'r':
                {
                  if (LocaleCompare(keyword,"radius") == 0)
                    {
                      geometry_info.rho=StringToDouble(value,
                        (char **) NULL);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'S':
                case 's':
                {
                  if (LocaleCompare(keyword,"sigma") == 0)
                    {
                      geometry_info.sigma=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          emboss_image=EmbossImage(msl_info->image[n],geometry_info.rho,
            geometry_info.sigma,&msl_info->image[n]->exception);
          if (emboss_image == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=emboss_image;
          break;
        }
      if (LocaleCompare((const char *) tag,"enhance") == 0)
        {
          Image
            *enhance_image;

          /*
            Enhance image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
            }
          enhance_image=EnhanceImage(msl_info->image[n],
            &msl_info->image[n]->exception);
          if (enhance_image == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=enhance_image;
          break;
        }
      if (LocaleCompare((const char *) tag,"equalize") == 0)
        {
          /*
            Equalize image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          (void) EqualizeImage(msl_info->image[n]);
          break;
        }
      ThrowMSLException(OptionError,"UnrecognizedElement",(const char *) tag);
    }
    case 'F':
    case 'f':
    {
      if (LocaleCompare((const char *) tag, "flatten") == 0)
      {
        if (msl_info->image[n] == (Image *) NULL)
        {
          ThrowMSLException(OptionError,"NoImagesDefined",
            (const char *) tag);
          break;
        }

        /* no attributes here */

        /* process the image */
        {
          Image
            *newImage;

          newImage=MergeImageLayers(msl_info->image[n],FlattenLayer,
            &msl_info->image[n]->exception);
          if (newImage == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=newImage;
          break;
        }
      }
      if (LocaleCompare((const char *) tag,"flip") == 0)
        {
          Image
            *flip_image;

          /*
            Flip image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
            }
          flip_image=FlipImage(msl_info->image[n],
            &msl_info->image[n]->exception);
          if (flip_image == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=flip_image;
          break;
        }
      if (LocaleCompare((const char *) tag,"flop") == 0)
        {
          Image
            *flop_image;

          /*
            Flop image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
            }
          flop_image=FlopImage(msl_info->image[n],
            &msl_info->image[n]->exception);
          if (flop_image == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=flop_image;
          break;
        }
      if (LocaleCompare((const char *) tag,"frame") == 0)
        {
          FrameInfo
            frame_info;

          Image
            *frame_image;

          /*
            Frame image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          (void) memset(&frame_info,0,sizeof(frame_info));
          SetGeometry(msl_info->image[n],&geometry);
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'C':
                case 'c':
                {
                  if (LocaleCompare(keyword,"compose") == 0)
                    {
                      option=ParseCommandOption(MagickComposeOptions,
                        MagickFalse,value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedComposeType",
                          value);
                      msl_info->image[n]->compose=(CompositeOperator) option;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'F':
                case 'f':
                {
                  if (LocaleCompare(keyword, "fill") == 0)
                    {
                      (void) QueryColorDatabase(value,
                        &msl_info->image[n]->matte_color,exception);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"geometry") == 0)
                    {
                      flags=ParsePageGeometry(msl_info->image[n],value,
                        &geometry,exception);
                      if ((flags & HeightValue) == 0)
                        geometry.height=geometry.width;
                      frame_info.width=geometry.width;
                      frame_info.height=geometry.height;
                      frame_info.outer_bevel=geometry.x;
                      frame_info.inner_bevel=geometry.y;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'H':
                case 'h':
                {
                  if (LocaleCompare(keyword,"height") == 0)
                    {
                      frame_info.height=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'I':
                case 'i':
                {
                  if (LocaleCompare(keyword,"inner") == 0)
                    {
                      frame_info.inner_bevel=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'O':
                case 'o':
                {
                  if (LocaleCompare(keyword,"outer") == 0)
                    {
                      frame_info.outer_bevel=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'W':
                case 'w':
                {
                  if (LocaleCompare(keyword,"width") == 0)
                    {
                      frame_info.width=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          frame_info.x=(ssize_t) frame_info.width;
          frame_info.y=(ssize_t) frame_info.height;
          frame_info.width=msl_info->image[n]->columns+2*frame_info.x;
          frame_info.height=msl_info->image[n]->rows+2*frame_info.y;
          frame_image=FrameImage(msl_info->image[n],&frame_info,
            &msl_info->image[n]->exception);
          if (frame_image == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=frame_image;
          break;
        }
      ThrowMSLException(OptionError,"UnrecognizedElement",(const char *) tag);
    }
    case 'G':
    case 'g':
    {
      if (LocaleCompare((const char *) tag,"gamma") == 0)
        {
          char
            gamma[MaxTextExtent];

          MagickPixelPacket
            pixel;

          /*
            Gamma image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          channel=UndefinedChannel;
          pixel.red=0.0;
          pixel.green=0.0;
          pixel.blue=0.0;
          *gamma='\0';
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'B':
                case 'b':
                {
                  if (LocaleCompare(keyword,"blue") == 0)
                    {
                      pixel.blue=StringToDouble(value,(char **) NULL);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'C':
                case 'c':
                {
                  if (LocaleCompare(keyword,"channel") == 0)
                    {
                      option=ParseChannelOption(value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedChannelType",
                          value);
                      channel=(ChannelType) option;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"gamma") == 0)
                    {
                      (void) CopyMagickString(gamma,value,MaxTextExtent);
                      break;
                    }
                  if (LocaleCompare(keyword,"green") == 0)
                    {
                      pixel.green=StringToDouble(value,(char **) NULL);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'R':
                case 'r':
                {
                  if (LocaleCompare(keyword,"red") == 0)
                    {
                      pixel.red=StringToDouble(value,(char **) NULL);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          if (*gamma == '\0')
            (void) FormatLocaleString(gamma,MaxTextExtent,"%g,%g,%g",
              (double) pixel.red,(double) pixel.green,(double) pixel.blue);
          switch (channel)
          {
            default:
            {
              (void) GammaImage(msl_info->image[n],gamma);
              break;
            }
            case RedChannel:
            {
              (void) GammaImageChannel(msl_info->image[n],RedChannel,pixel.red);
              break;
            }
            case GreenChannel:
            {
              (void) GammaImageChannel(msl_info->image[n],GreenChannel,
                pixel.green);
              break;
            }
            case BlueChannel:
            {
              (void) GammaImageChannel(msl_info->image[n],BlueChannel,
                pixel.blue);
              break;
            }
          }
          break;
        }
      else if (LocaleCompare((const char *) tag,"get") == 0)
        {
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          if (attributes == (const xmlChar **) NULL)
            break;
          for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
          {
            keyword=(const char *) attributes[i++];
            CloneString(&value,(const char *) attributes[i]);
            (void) CopyMagickString(key,value,MaxTextExtent);
            switch (*keyword)
            {
              case 'H':
              case 'h':
              {
                if (LocaleCompare(keyword,"height") == 0)
                  {
                    (void) FormatLocaleString(value,MaxTextExtent,"%.20g",
                      (double) msl_info->image[n]->rows);
                    (void) SetImageProperty(msl_info->attributes[n],key,value);
                    break;
                  }
                ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
              }
              case 'W':
              case 'w':
              {
                if (LocaleCompare(keyword,"width") == 0)
                  {
                    (void) FormatLocaleString(value,MaxTextExtent,"%.20g",
                      (double) msl_info->image[n]->columns);
                    (void) SetImageProperty(msl_info->attributes[n],key,value);
                    break;
                  }
                ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
              }
              default:
              {
                ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
                break;
              }
            }
          }
          break;
        }
    else if (LocaleCompare((const char *) tag, "group") == 0)
    {
      msl_info->number_groups++;
      msl_info->group_info=(MSLGroupInfo *) ResizeQuantumMemory(
        msl_info->group_info,msl_info->number_groups+1UL,
        sizeof(*msl_info->group_info));
      break;
    }
      ThrowMSLException(OptionError,"UnrecognizedElement",(const char *) tag);
    }
    case 'I':
    case 'i':
    {
      if (LocaleCompare((const char *) tag,"image") == 0)
        {
          MSLPushImage(msl_info,(Image *) NULL);
          if (attributes == (const xmlChar **) NULL)
            break;
          for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
          {
            keyword=(const char *) attributes[i++];
            attribute=InterpretImageProperties(msl_info->image_info[n],
              msl_info->attributes[n],(const char *) attributes[i]);
            CloneString(&value,attribute);
            attribute=DestroyString(attribute);
            switch (*keyword)
            {
              case 'C':
              case 'c':
              {
                if (LocaleCompare(keyword,"color") == 0)
                  {
                    Image
                      *next_image;

                    (void) CopyMagickString(msl_info->image_info[n]->filename,
                      "xc:",MaxTextExtent);
                    (void) ConcatenateMagickString(msl_info->image_info[n]->
                      filename,value,MaxTextExtent);
                    next_image=ReadImage(msl_info->image_info[n],exception);
                    CatchException(exception);
                    if (next_image == (Image *) NULL)
                      continue;
                    if (msl_info->image[n] == (Image *) NULL)
                      msl_info->image[n]=next_image;
                    else
                      {
                        register Image
                          *p;

                        /*
                          Link image into image list.
                        */
                        p=msl_info->image[n];
                        while (p->next != (Image *) NULL)
                          p=GetNextImageInList(p);
                        next_image->previous=p;
                        p->next=next_image;
                      }
                    break;
                  }
                (void) SetMSLAttributes(msl_info,keyword,value);
                break;
              }
              default:
              {
                (void) SetMSLAttributes(msl_info,keyword,value);
                break;
              }
            }
          }
          break;
        }
      if (LocaleCompare((const char *) tag,"implode") == 0)
        {
          Image
            *implode_image;

          /*
            Implode image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'A':
                case 'a':
                {
                  if (LocaleCompare(keyword,"amount") == 0)
                    {
                      geometry_info.rho=StringToDouble(value,
                        (char **) NULL);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"geometry") == 0)
                    {
                      flags=ParseGeometry(value,&geometry_info);
                      if ((flags & SigmaValue) == 0)
                        geometry_info.sigma=1.0;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          implode_image=ImplodeImage(msl_info->image[n],geometry_info.rho,
            &msl_info->image[n]->exception);
          if (implode_image == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=implode_image;
          break;
        }
      ThrowMSLException(OptionError,"UnrecognizedElement",(const char *) tag);
    }
    case 'L':
    case 'l':
    {
      if (LocaleCompare((const char *) tag,"label") == 0)
        break;
      if (LocaleCompare((const char *) tag, "level") == 0)
      {
        double
          levelBlack = 0, levelGamma = 1, levelWhite = QuantumRange;

        if (msl_info->image[n] == (Image *) NULL)
        {
          ThrowMSLException(OptionError,"NoImagesDefined",
            (const char *) tag);
          break;
        }
        if (attributes == (const xmlChar **) NULL)
          break;
        for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
        {
          keyword=(const char *) attributes[i++];
          CloneString(&value,(const char *) attributes[i]);
          (void) CopyMagickString(key,value,MaxTextExtent);
          switch (*keyword)
          {
            case 'B':
            case 'b':
            {
              if (LocaleCompare(keyword,"black") == 0)
              {
                levelBlack = StringToDouble(value,(char **) NULL);
                break;
              }
              ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
              break;
            }
            case 'G':
            case 'g':
            {
              if (LocaleCompare(keyword,"gamma") == 0)
              {
                levelGamma = StringToDouble(value,(char **) NULL);
                break;
              }
              ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
              break;
            }
            case 'W':
            case 'w':
            {
              if (LocaleCompare(keyword,"white") == 0)
              {
                levelWhite = StringToDouble(value,(char **) NULL);
                break;
              }
              ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
              break;
            }
            default:
            {
              ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
              break;
            }
          }
        }

        /* process image */
        {
          char level[MaxTextExtent + 1];
          (void) FormatLocaleString(level,MaxTextExtent,"%3.6f/%3.6f/%3.6f/",
            levelBlack,levelGamma,levelWhite);
          LevelImage ( msl_info->image[n], level );
          break;
        }
      }
    }
    case 'M':
    case 'm':
    {
      if (LocaleCompare((const char *) tag,"magnify") == 0)
        {
          Image
            *magnify_image;

          /*
            Magnify image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
            }
          magnify_image=MagnifyImage(msl_info->image[n],
            &msl_info->image[n]->exception);
          if (magnify_image == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=magnify_image;
          break;
        }
      if (LocaleCompare((const char *) tag,"map") == 0)
        {
          Image
            *affinity_image;

          MagickBooleanType
            dither;

          QuantizeInfo
            *quantize_info;

          /*
            Map image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          affinity_image=NewImageList();
          dither=MagickFalse;
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'D':
                case 'd':
                {
                  if (LocaleCompare(keyword,"dither") == 0)
                    {
                      option=ParseCommandOption(MagickBooleanOptions,
                        MagickFalse,value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedBooleanType",
                          value);
                      dither=(MagickBooleanType) option;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'I':
                case 'i':
                {
                  if (LocaleCompare(keyword,"image") == 0)
                    for (j=0; j < msl_info->n; j++)
                    {
                      const char
                        *attribute;

                      attribute=GetImageProperty(msl_info->attributes[j],"id");
                      if ((attribute != (const char *) NULL)  &&
                          (LocaleCompare(attribute,value) == 0))
                        {
                          affinity_image=CloneImage(msl_info->image[j],0,0,
                            MagickFalse,exception);
                          break;
                        }
                    }
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          quantize_info=AcquireQuantizeInfo(msl_info->image_info[n]);
          quantize_info->dither=dither;
          (void) RemapImages(quantize_info,msl_info->image[n],
            affinity_image);
          quantize_info=DestroyQuantizeInfo(quantize_info);
          affinity_image=DestroyImage(affinity_image);
          break;
        }
      if (LocaleCompare((const char *) tag,"matte-floodfill") == 0)
        {
          double
            opacity;

          MagickPixelPacket
            target;

          PaintMethod
            paint_method;

          /*
            Matte floodfill image.
          */
          opacity=0.0;
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          SetGeometry(msl_info->image[n],&geometry);
          paint_method=FloodfillMethod;
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'B':
                case 'b':
                {
                  if (LocaleCompare(keyword,"bordercolor") == 0)
                    {
                      (void) QueryMagickColor(value,&target,exception);
                      paint_method=FillToBorderMethod;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'F':
                case 'f':
                {
                  if (LocaleCompare(keyword,"fuzz") == 0)
                    {
                      msl_info->image[n]->fuzz=StringToDouble(value,
                        (char **) NULL);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"geometry") == 0)
                    {
                      flags=ParsePageGeometry(msl_info->image[n],value,
                        &geometry,exception);
                      if ((flags & HeightValue) == 0)
                        geometry.height=geometry.width;
                      (void) GetOneVirtualMagickPixel(msl_info->image[n],
                        geometry.x,geometry.y,&target,exception);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'O':
                case 'o':
                {
                  if (LocaleCompare(keyword,"opacity") == 0)
                    {
                      opacity=StringToDouble(value,(char **) NULL);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'X':
                case 'x':
                {
                  if (LocaleCompare(keyword,"x") == 0)
                    {
                      geometry.x=StringToLong(value);
                      (void) GetOneVirtualMagickPixel(msl_info->image[n],
                        geometry.x,geometry.y,&target,exception);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'Y':
                case 'y':
                {
                  if (LocaleCompare(keyword,"y") == 0)
                    {
                      geometry.y=StringToLong(value);
                      (void) GetOneVirtualMagickPixel(msl_info->image[n],
                        geometry.x,geometry.y,&target,exception);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          draw_info=CloneDrawInfo(msl_info->image_info[n],
            msl_info->draw_info[n]);
          draw_info->fill.opacity=ClampToQuantum(opacity);
          (void) FloodfillPaintImage(msl_info->image[n],OpacityChannel,
            draw_info,&target,geometry.x,geometry.y,
            paint_method == FloodfillMethod ? MagickFalse : MagickTrue);
          draw_info=DestroyDrawInfo(draw_info);
          break;
        }
      if (LocaleCompare((const char *) tag,"median-filter") == 0)
        {
          Image
            *median_image;

          /*
            Median-filter image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"geometry") == 0)
                    {
                      flags=ParseGeometry(value,&geometry_info);
                      if ((flags & SigmaValue) == 0)
                        geometry_info.sigma=1.0;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'R':
                case 'r':
                {
                  if (LocaleCompare(keyword,"radius") == 0)
                    {
                      geometry_info.rho=StringToDouble(value,
                        (char **) NULL);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          median_image=StatisticImage(msl_info->image[n],MedianStatistic,
            (size_t) geometry_info.rho,(size_t) geometry_info.sigma,
            &msl_info->image[n]->exception);
          if (median_image == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=median_image;
          break;
        }
      if (LocaleCompare((const char *) tag,"minify") == 0)
        {
          Image
            *minify_image;

          /*
            Minify image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
            }
          minify_image=MinifyImage(msl_info->image[n],
            &msl_info->image[n]->exception);
          if (minify_image == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=minify_image;
          break;
        }
      if (LocaleCompare((const char *) tag,"msl") == 0 )
        break;
      if (LocaleCompare((const char *) tag,"modulate") == 0)
        {
          char
            modulate[MaxTextExtent];

          /*
            Modulate image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          geometry_info.rho=100.0;
          geometry_info.sigma=100.0;
          geometry_info.xi=100.0;
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'B':
                case 'b':
                {
                  if (LocaleCompare(keyword,"blackness") == 0)
                    {
                      geometry_info.rho=StringToDouble(value,
                        (char **) NULL);
                      break;
                    }
                  if (LocaleCompare(keyword,"brightness") == 0)
                    {
                      geometry_info.rho=StringToDouble(value,
                        (char **) NULL);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'F':
                case 'f':
                {
                  if (LocaleCompare(keyword,"factor") == 0)
                    {
                      flags=ParseGeometry(value,&geometry_info);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'H':
                case 'h':
                {
                  if (LocaleCompare(keyword,"hue") == 0)
                    {
                      geometry_info.xi=StringToDouble(value,
                        (char **) NULL);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'L':
                case 'l':
                {
                  if (LocaleCompare(keyword,"lightness") == 0)
                    {
                      geometry_info.rho=StringToDouble(value,
                        (char **) NULL);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'S':
                case 's':
                {
                  if (LocaleCompare(keyword,"saturation") == 0)
                    {
                      geometry_info.sigma=StringToDouble(value,
                        (char **) NULL);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'W':
                case 'w':
                {
                  if (LocaleCompare(keyword,"whiteness") == 0)
                    {
                      geometry_info.sigma=StringToDouble(value,
                        (char **) NULL);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          (void) FormatLocaleString(modulate,MaxTextExtent,"%g,%g,%g",
            geometry_info.rho,geometry_info.sigma,geometry_info.xi);
          (void) ModulateImage(msl_info->image[n],modulate);
          break;
        }
      ThrowMSLException(OptionError,"UnrecognizedElement",(const char *) tag);
    }
    case 'N':
    case 'n':
    {
      if (LocaleCompare((const char *) tag,"negate") == 0)
        {
          MagickBooleanType
            gray;

          /*
            Negate image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          gray=MagickFalse;
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'C':
                case 'c':
                {
                  if (LocaleCompare(keyword,"channel") == 0)
                    {
                      option=ParseChannelOption(value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedChannelType",
                          value);
                      channel=(ChannelType) option;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"gray") == 0)
                    {
                      option=ParseCommandOption(MagickBooleanOptions,MagickFalse,
                        value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedBooleanType",
                          value);
                      gray=(MagickBooleanType) option;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          (void) NegateImageChannel(msl_info->image[n],channel,gray);
          break;
        }
      if (LocaleCompare((const char *) tag,"normalize") == 0)
        {
          /*
            Normalize image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'C':
                case 'c':
                {
                  if (LocaleCompare(keyword,"channel") == 0)
                    {
                      option=ParseChannelOption(value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedChannelType",
                          value);
                      channel=(ChannelType) option;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          (void) NormalizeImageChannel(msl_info->image[n],channel);
          break;
        }
      ThrowMSLException(OptionError,"UnrecognizedElement",(const char *) tag);
    }
    case 'O':
    case 'o':
    {
      if (LocaleCompare((const char *) tag,"oil-paint") == 0)
        {
          Image
            *paint_image;

          /*
            Oil-paint image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"geometry") == 0)
                    {
                      flags=ParseGeometry(value,&geometry_info);
                      if ((flags & SigmaValue) == 0)
                        geometry_info.sigma=1.0;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'R':
                case 'r':
                {
                  if (LocaleCompare(keyword,"radius") == 0)
                    {
                      geometry_info.rho=StringToDouble(value,
                        (char **) NULL);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          paint_image=OilPaintImage(msl_info->image[n],geometry_info.rho,
            &msl_info->image[n]->exception);
          if (paint_image == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=paint_image;
          break;
        }
      if (LocaleCompare((const char *) tag,"opaque") == 0)
        {
          MagickPixelPacket
            fill_color,
            target;

          /*
            Opaque image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          (void) QueryMagickColor("none",&target,exception);
          (void) QueryMagickColor("none",&fill_color,exception);
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'C':
                case 'c':
                {
                  if (LocaleCompare(keyword,"channel") == 0)
                    {
                      option=ParseChannelOption(value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedChannelType",
                          value);
                      channel=(ChannelType) option;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'F':
                case 'f':
                {
                  if (LocaleCompare(keyword,"fill") == 0)
                    {
                      (void) QueryMagickColor(value,&fill_color,exception);
                      break;
                    }
                  if (LocaleCompare(keyword,"fuzz") == 0)
                    {
                      msl_info->image[n]->fuzz=StringToDouble(value,
                        (char **) NULL);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          (void) OpaquePaintImageChannel(msl_info->image[n],channel,
            &target,&fill_color,MagickFalse);
          break;
        }
      ThrowMSLException(OptionError,"UnrecognizedElement",(const char *) tag);
    }
    case 'P':
    case 'p':
    {
      if (LocaleCompare((const char *) tag,"print") == 0)
        {
          if (attributes == (const xmlChar **) NULL)
            break;
          for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
          {
            keyword=(const char *) attributes[i++];
            attribute=InterpretImageProperties(msl_info->image_info[n],
              msl_info->attributes[n],(const char *) attributes[i]);
            CloneString(&value,attribute);
            attribute=DestroyString(attribute);
            switch (*keyword)
            {
              case 'O':
              case 'o':
              {
                if (LocaleCompare(keyword,"output") == 0)
                  {
                    (void) FormatLocaleFile(stdout,"%s",value);
                    break;
                  }
                ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
                break;
              }
              default:
              {
                ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
                break;
              }
            }
          }
          break;
        }
        if (LocaleCompare((const char *) tag, "profile") == 0)
          {
            if (msl_info->image[n] == (Image *) NULL)
              {
                ThrowMSLException(OptionError,"NoImagesDefined",
                  (const char *) tag);
                break;
              }
            if (attributes == (const xmlChar **) NULL)
              break;
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              const char
                *name;

              const StringInfo
                *profile;

              Image
                *profile_image;

              ImageInfo
                *profile_info;

              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              if (*keyword == '!')
                {
                  /*
                    Remove a profile from the image.
                  */
                  (void) ProfileImage(msl_info->image[n],keyword,
                    (const unsigned char *) NULL,0,MagickTrue);
                  continue;
                }
              /*
                Associate a profile with the image.
              */
              profile_info=CloneImageInfo(msl_info->image_info[n]);
              profile=GetImageProfile(msl_info->image[n],"iptc");
              if (profile != (StringInfo *) NULL)
                profile_info->profile=(void *) CloneStringInfo(profile);
              profile_image=GetImageCache(profile_info,keyword,exception);
              profile_info=DestroyImageInfo(profile_info);
              if (profile_image == (Image *) NULL)
                {
                  char
                    name[MaxTextExtent],
                    filename[MaxTextExtent];

                  register char
                    *p;

                  StringInfo
                    *profile;

                  (void) CopyMagickString(filename,keyword,MaxTextExtent);
                  (void) CopyMagickString(name,keyword,MaxTextExtent);
                  for (p=filename; *p != '\0'; p++)
                    if ((*p == ':') && (IsPathDirectory(keyword) < 0) &&
                        (IsPathAccessible(keyword) == MagickFalse))
                      {
                        register char
                          *q;

                        /*
                          Look for profile name (e.g. name:profile).
                        */
                        (void) CopyMagickString(name,filename,(size_t)
                          (p-filename+1));
                        for (q=filename; *q != '\0'; q++)
                          *q=(*++p);
                        break;
                      }
                  profile=FileToStringInfo(filename,~0UL,exception);
                  if (profile != (StringInfo *) NULL)
                    {
                      (void) ProfileImage(msl_info->image[n],name,
                        GetStringInfoDatum(profile),(size_t)
                        GetStringInfoLength(profile),MagickFalse);
                      profile=DestroyStringInfo(profile);
                    }
                  continue;
                }
              ResetImageProfileIterator(profile_image);
              name=GetNextImageProfile(profile_image);
              while (name != (const char *) NULL)
              {
                profile=GetImageProfile(profile_image,name);
                if (profile != (StringInfo *) NULL)
                  (void) ProfileImage(msl_info->image[n],name,
                    GetStringInfoDatum(profile),(size_t)
                    GetStringInfoLength(profile),MagickFalse);
                name=GetNextImageProfile(profile_image);
              }
              profile_image=DestroyImage(profile_image);
            }
            break;
          }
      ThrowMSLException(OptionError,"UnrecognizedElement",(const char *) tag);
    }
    case 'Q':
    case 'q':
    {
      if (LocaleCompare((const char *) tag,"quantize") == 0)
        {
          QuantizeInfo
            quantize_info;

          /*
            Quantize image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          GetQuantizeInfo(&quantize_info);
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'C':
                case 'c':
                {
                  if (LocaleCompare(keyword,"colors") == 0)
                    {
                      quantize_info.number_colors=StringToLong(value);
                      break;
                    }
                  if (LocaleCompare(keyword,"colorspace") == 0)
                    {
                      option=ParseCommandOption(MagickColorspaceOptions,
                        MagickFalse,value);
                      if (option < 0)
                        ThrowMSLException(OptionError,
                          "UnrecognizedColorspaceType",value);
                      quantize_info.colorspace=(ColorspaceType) option;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'D':
                case 'd':
                {
                  if (LocaleCompare(keyword,"dither") == 0)
                    {
                      option=ParseCommandOption(MagickBooleanOptions,MagickFalse,
                        value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedBooleanType",
                          value);
                      quantize_info.dither=(MagickBooleanType) option;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'M':
                case 'm':
                {
                  if (LocaleCompare(keyword,"measure") == 0)
                    {
                      option=ParseCommandOption(MagickBooleanOptions,MagickFalse,
                        value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedBooleanType",
                          value);
                      quantize_info.measure_error=(MagickBooleanType) option;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'T':
                case 't':
                {
                  if (LocaleCompare(keyword,"treedepth") == 0)
                    {
                      quantize_info.tree_depth=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          (void) QuantizeImage(&quantize_info,msl_info->image[n]);
          break;
        }
      if (LocaleCompare((const char *) tag,"query-font-metrics") == 0)
        {
          char
            text[MaxTextExtent];

          MagickBooleanType
            status;

          TypeMetric
            metrics;

          /*
            Query font metrics.
          */
          draw_info=CloneDrawInfo(msl_info->image_info[n],
            msl_info->draw_info[n]);
          angle=0.0;
          current=draw_info->affine;
          GetAffineMatrix(&affine);
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'A':
                case 'a':
                {
                  if (LocaleCompare(keyword,"affine") == 0)
                    {
                      char
                        *p;

                      p=value;
                      draw_info->affine.sx=StringToDouble(p,&p);
                      if (*p ==',')
                        p++;
                      draw_info->affine.rx=StringToDouble(p,&p);
                      if (*p ==',')
                        p++;
                      draw_info->affine.ry=StringToDouble(p,&p);
                      if (*p ==',')
                        p++;
                      draw_info->affine.sy=StringToDouble(p,&p);
                      if (*p ==',')
                        p++;
                      draw_info->affine.tx=StringToDouble(p,&p);
                      if (*p ==',')
                        p++;
                      draw_info->affine.ty=StringToDouble(p,&p);
                      break;
                    }
                  if (LocaleCompare(keyword,"align") == 0)
                    {
                      option=ParseCommandOption(MagickAlignOptions,MagickFalse,
                        value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedAlignType",
                          value);
                      draw_info->align=(AlignType) option;
                      break;
                    }
                  if (LocaleCompare(keyword,"antialias") == 0)
                    {
                      option=ParseCommandOption(MagickBooleanOptions,MagickFalse,
                        value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedBooleanType",
                          value);
                      draw_info->stroke_antialias=(MagickBooleanType) option;
                      draw_info->text_antialias=(MagickBooleanType) option;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'D':
                case 'd':
                {
                  if (LocaleCompare(keyword,"density") == 0)
                    {
                      CloneString(&draw_info->density,value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'E':
                case 'e':
                {
                  if (LocaleCompare(keyword,"encoding") == 0)
                    {
                      CloneString(&draw_info->encoding,value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'F':
                case 'f':
                {
                  if (LocaleCompare(keyword, "fill") == 0)
                    {
                      (void) QueryColorDatabase(value,&draw_info->fill,
                        exception);
                      break;
                    }
                  if (LocaleCompare(keyword,"family") == 0)
                    {
                      CloneString(&draw_info->family,value);
                      break;
                    }
                  if (LocaleCompare(keyword,"font") == 0)
                    {
                      CloneString(&draw_info->font,value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"geometry") == 0)
                    {
                      flags=ParsePageGeometry(msl_info->image[n],value,
                        &geometry,exception);
                      if ((flags & HeightValue) == 0)
                        geometry.height=geometry.width;
                      break;
                    }
                  if (LocaleCompare(keyword,"gravity") == 0)
                    {
                      option=ParseCommandOption(MagickGravityOptions,MagickFalse,
                        value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedGravityType",
                          value);
                      draw_info->gravity=(GravityType) option;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'P':
                case 'p':
                {
                  if (LocaleCompare(keyword,"pointsize") == 0)
                    {
                      draw_info->pointsize=StringToDouble(value,
                        (char **) NULL);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'R':
                case 'r':
                {
                  if (LocaleCompare(keyword,"rotate") == 0)
                    {
                      angle=StringToDouble(value,(char **) NULL);
                      affine.sx=cos(DegreesToRadians(fmod(angle,360.0)));
                      affine.rx=sin(DegreesToRadians(fmod(angle,360.0)));
                      affine.ry=(-sin(DegreesToRadians(fmod(angle,360.0))));
                      affine.sy=cos(DegreesToRadians(fmod(angle,360.0)));
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'S':
                case 's':
                {
                  if (LocaleCompare(keyword,"scale") == 0)
                    {
                      flags=ParseGeometry(value,&geometry_info);
                      if ((flags & SigmaValue) == 0)
                        geometry_info.sigma=1.0;
                      affine.sx=geometry_info.rho;
                      affine.sy=geometry_info.sigma;
                      break;
                    }
                  if (LocaleCompare(keyword,"skewX") == 0)
                    {
                      angle=StringToDouble(value,(char **) NULL);
                      affine.ry=cos(DegreesToRadians(fmod(angle,360.0)));
                      break;
                    }
                  if (LocaleCompare(keyword,"skewY") == 0)
                    {
                      angle=StringToDouble(value,(char **) NULL);
                      affine.rx=cos(DegreesToRadians(fmod(angle,360.0)));
                      break;
                    }
                  if (LocaleCompare(keyword,"stretch") == 0)
                    {
                      option=ParseCommandOption(MagickStretchOptions,MagickFalse,
                        value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedStretchType",
                          value);
                      draw_info->stretch=(StretchType) option;
                      break;
                    }
                  if (LocaleCompare(keyword, "stroke") == 0)
                    {
                      (void) QueryColorDatabase(value,&draw_info->stroke,
                        exception);
                      break;
                    }
                  if (LocaleCompare(keyword,"strokewidth") == 0)
                    {
                      draw_info->stroke_width=StringToLong(value);
                      break;
                    }
                  if (LocaleCompare(keyword,"style") == 0)
                    {
                      option=ParseCommandOption(MagickStyleOptions,MagickFalse,
                        value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedStyleType",
                          value);
                      draw_info->style=(StyleType) option;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'T':
                case 't':
                {
                  if (LocaleCompare(keyword,"text") == 0)
                    {
                      CloneString(&draw_info->text,value);
                      break;
                    }
                  if (LocaleCompare(keyword,"translate") == 0)
                    {
                      flags=ParseGeometry(value,&geometry_info);
                      if ((flags & SigmaValue) == 0)
                        geometry_info.sigma=1.0;
                      affine.tx=geometry_info.rho;
                      affine.ty=geometry_info.sigma;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'U':
                case 'u':
                {
                  if (LocaleCompare(keyword, "undercolor") == 0)
                    {
                      (void) QueryColorDatabase(value,&draw_info->undercolor,
                        exception);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'W':
                case 'w':
                {
                  if (LocaleCompare(keyword,"weight") == 0)
                    {
                      draw_info->weight=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'X':
                case 'x':
                {
                  if (LocaleCompare(keyword,"x") == 0)
                    {
                      geometry.x=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'Y':
                case 'y':
                {
                  if (LocaleCompare(keyword,"y") == 0)
                    {
                      geometry.y=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          (void) FormatLocaleString(text,MaxTextExtent,
            "%.20gx%.20g%+.20g%+.20g",(double) geometry.width,(double)
            geometry.height,(double) geometry.x,(double) geometry.y);
          CloneString(&draw_info->geometry,text);
          draw_info->affine.sx=affine.sx*current.sx+affine.ry*current.rx;
          draw_info->affine.rx=affine.rx*current.sx+affine.sy*current.rx;
          draw_info->affine.ry=affine.sx*current.ry+affine.ry*current.sy;
          draw_info->affine.sy=affine.rx*current.ry+affine.sy*current.sy;
          draw_info->affine.tx=affine.sx*current.tx+affine.ry*current.ty+
            affine.tx;
          draw_info->affine.ty=affine.rx*current.tx+affine.sy*current.ty+
            affine.ty;
          status=GetTypeMetrics(msl_info->attributes[n],draw_info,&metrics);
          if (status != MagickFalse)
            {
              Image
                *image;

              image=msl_info->attributes[n];
              FormatImageProperty(image,"msl:font-metrics.pixels_per_em.x",
                "%g",metrics.pixels_per_em.x);
              FormatImageProperty(image,"msl:font-metrics.pixels_per_em.y",
                "%g",metrics.pixels_per_em.y);
              FormatImageProperty(image,"msl:font-metrics.ascent","%g",
                metrics.ascent);
              FormatImageProperty(image,"msl:font-metrics.descent","%g",
                metrics.descent);
              FormatImageProperty(image,"msl:font-metrics.width","%g",
                metrics.width);
              FormatImageProperty(image,"msl:font-metrics.height","%g",
                metrics.height);
              FormatImageProperty(image,"msl:font-metrics.max_advance","%g",
                metrics.max_advance);
              FormatImageProperty(image,"msl:font-metrics.bounds.x1","%g",
                metrics.bounds.x1);
              FormatImageProperty(image,"msl:font-metrics.bounds.y1","%g",
                metrics.bounds.y1);
              FormatImageProperty(image,"msl:font-metrics.bounds.x2","%g",
                metrics.bounds.x2);
              FormatImageProperty(image,"msl:font-metrics.bounds.y2","%g",
                metrics.bounds.y2);
              FormatImageProperty(image,"msl:font-metrics.origin.x","%g",
                metrics.origin.x);
              FormatImageProperty(image,"msl:font-metrics.origin.y","%g",
                metrics.origin.y);
            }
          draw_info=DestroyDrawInfo(draw_info);
          break;
        }
      ThrowMSLException(OptionError,"UnrecognizedElement",(const char *) tag);
    }
    case 'R':
    case 'r':
    {
      if (LocaleCompare((const char *) tag,"raise") == 0)
        {
          MagickBooleanType
            raise;

          /*
            Raise image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          raise=MagickFalse;
          SetGeometry(msl_info->image[n],&geometry);
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"geometry") == 0)
                    {
                      flags=ParsePageGeometry(msl_info->image[n],value,
                        &geometry,exception);
                      if ((flags & HeightValue) == 0)
                        geometry.height=geometry.width;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'H':
                case 'h':
                {
                  if (LocaleCompare(keyword,"height") == 0)
                    {
                      geometry.height=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'R':
                case 'r':
                {
                  if (LocaleCompare(keyword,"raise") == 0)
                    {
                      option=ParseCommandOption(MagickBooleanOptions,MagickFalse,
                        value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedNoiseType",
                          value);
                      raise=(MagickBooleanType) option;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'W':
                case 'w':
                {
                  if (LocaleCompare(keyword,"width") == 0)
                    {
                      geometry.width=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          (void) RaiseImage(msl_info->image[n],&geometry,raise);
          break;
        }
      if (LocaleCompare((const char *) tag,"read") == 0)
        {
          if (attributes == (const xmlChar **) NULL)
            break;
          for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
          {
            keyword=(const char *) attributes[i++];
            attribute=InterpretImageProperties(msl_info->image_info[n],
              msl_info->attributes[n],(const char *) attributes[i]);
            CloneString(&value,attribute);
            attribute=DestroyString(attribute);
            switch (*keyword)
            {
              case 'F':
              case 'f':
              {
                if (LocaleCompare(keyword,"filename") == 0)
                  {
                    Image
                      *image;

                    if (value == (char *) NULL)
                      break;
                    (void) CopyMagickString(msl_info->image_info[n]->filename,
                      value,MaxTextExtent);
                    image=ReadImage(msl_info->image_info[n],exception);
                    CatchException(exception);
                    if (image == (Image *) NULL)
                      continue;
                    AppendImageToList(&msl_info->image[n],image);
                    break;
                  }
                (void) SetMSLAttributes(msl_info,keyword,value);
                break;
              }
              default:
              {
                (void) SetMSLAttributes(msl_info,keyword,value);
                break;
              }
            }
          }
          break;
        }
      if (LocaleCompare((const char *) tag,"reduce-noise") == 0)
        {
          Image
            *paint_image;

          /*
            Reduce-noise image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"geometry") == 0)
                    {
                      flags=ParseGeometry(value,&geometry_info);
                      if ((flags & SigmaValue) == 0)
                        geometry_info.sigma=1.0;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'R':
                case 'r':
                {
                  if (LocaleCompare(keyword,"radius") == 0)
                    {
                      geometry_info.rho=StringToDouble(value,
                        (char **) NULL);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          paint_image=StatisticImage(msl_info->image[n],NonpeakStatistic,
            (size_t) geometry_info.rho,(size_t) geometry_info.sigma,
            &msl_info->image[n]->exception);
          if (paint_image == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=paint_image;
          break;
        }
      else if (LocaleCompare((const char *) tag,"repage") == 0)
      {
        /* init the values */
        width=msl_info->image[n]->page.width;
        height=msl_info->image[n]->page.height;
        x=msl_info->image[n]->page.x;
        y=msl_info->image[n]->page.y;

        if (msl_info->image[n] == (Image *) NULL)
        {
          ThrowMSLException(OptionError,"NoImagesDefined",
            (const char *) tag);
          break;
        }
        if (attributes == (const xmlChar **) NULL)
        break;
        for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
        {
        keyword=(const char *) attributes[i++];
        attribute=InterpretImageProperties(msl_info->image_info[n],
          msl_info->attributes[n],(const char *) attributes[i]);
        CloneString(&value,attribute);
        attribute=DestroyString(attribute);
        switch (*keyword)
        {
          case 'G':
          case 'g':
          {
          if (LocaleCompare(keyword,"geometry") == 0)
            {
              int
                flags;

              RectangleInfo
                geometry;

            flags=ParseAbsoluteGeometry(value,&geometry);
            if ((flags & WidthValue) != 0)
              {
                if ((flags & HeightValue) == 0)
                  geometry.height=geometry.width;
                width=geometry.width;
                height=geometry.height;
              }
            if ((flags & AspectValue) != 0)
              {
                if ((flags & XValue) != 0)
                  x+=geometry.x;
                if ((flags & YValue) != 0)
                  y+=geometry.y;
              }
            else
              {
                if ((flags & XValue) != 0)
                  {
                    x=geometry.x;
                    if ((width == 0) && (geometry.x > 0))
                      width=msl_info->image[n]->columns+geometry.x;
                  }
                if ((flags & YValue) != 0)
                  {
                    y=geometry.y;
                    if ((height == 0) && (geometry.y > 0))
                      height=msl_info->image[n]->rows+geometry.y;
                  }
              }
            break;
            }
          ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
          break;
          }
          case 'H':
          case 'h':
          {
          if (LocaleCompare(keyword,"height") == 0)
            {
            height = StringToLong( value );
            break;
            }
          ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
          break;
          }
          case 'W':
          case 'w':
          {
          if (LocaleCompare(keyword,"width") == 0)
            {
            width = StringToLong( value );
            break;
            }
          ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
          break;
          }
          case 'X':
          case 'x':
          {
          if (LocaleCompare(keyword,"x") == 0)
            {
            x = StringToLong( value );
            break;
            }
          ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
          break;
          }
          case 'Y':
          case 'y':
          {
          if (LocaleCompare(keyword,"y") == 0)
            {
            y = StringToLong( value );
            break;
            }
          ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
          break;
          }
          default:
          {
          ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
          break;
          }
        }
        }

         msl_info->image[n]->page.width=width;
         msl_info->image[n]->page.height=height;
         msl_info->image[n]->page.x=x;
         msl_info->image[n]->page.y=y;
        break;
      }
    else if (LocaleCompare((const char *) tag,"resample") == 0)
    {
      double
        x_resolution,
        y_resolution;

      if (msl_info->image[n] == (Image *) NULL)
        {
          ThrowMSLException(OptionError,"NoImagesDefined",
            (const char *) tag);
          break;
        }
      if (attributes == (const xmlChar **) NULL)
        break;
      x_resolution=DefaultResolution;
      y_resolution=DefaultResolution;
      for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
      {
        keyword=(const char *) attributes[i++];
        attribute=InterpretImageProperties(msl_info->image_info[n],
          msl_info->attributes[n],(const char *) attributes[i]);
        CloneString(&value,attribute);
        attribute=DestroyString(attribute);
        switch (*keyword)
        {
          case 'b':
          {
            if (LocaleCompare(keyword,"blur") == 0)
              {
                msl_info->image[n]->blur=StringToDouble(value,
                        (char **) NULL);
                break;
              }
            ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
            break;
          }
          case 'G':
          case 'g':
          {
            if (LocaleCompare(keyword,"geometry") == 0)
              {
                ssize_t
                  flags;

                flags=ParseGeometry(value,&geometry_info);
                if ((flags & SigmaValue) == 0)
                  geometry_info.sigma*=geometry_info.rho;
                x_resolution=geometry_info.rho;
                y_resolution=geometry_info.sigma;
                break;
              }
            ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
            break;
          }
          case 'X':
          case 'x':
          {
            if (LocaleCompare(keyword,"x-resolution") == 0)
              {
                x_resolution=StringToDouble(value,(char **) NULL);
                break;
              }
            ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
            break;
          }
          case 'Y':
          case 'y':
          {
            if (LocaleCompare(keyword,"y-resolution") == 0)
              {
                y_resolution=StringToDouble(value,(char **) NULL);
                break;
              }
            ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
            break;
          }
          default:
          {
            ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
            break;
          }
        }
      }
      /*
        Resample image.
      */
      {
        double
          factor;

        Image
          *resample_image;

        factor=1.0;
        if (msl_info->image[n]->units == PixelsPerCentimeterResolution)
          factor=2.54;
        width=(size_t) (x_resolution*msl_info->image[n]->columns/
          (factor*(msl_info->image[n]->x_resolution == 0.0 ? DefaultResolution :
          msl_info->image[n]->x_resolution))+0.5);
        height=(size_t) (y_resolution*msl_info->image[n]->rows/
          (factor*(msl_info->image[n]->y_resolution == 0.0 ? DefaultResolution :
          msl_info->image[n]->y_resolution))+0.5);
        resample_image=ResizeImage(msl_info->image[n],width,height,
          msl_info->image[n]->filter,msl_info->image[n]->blur,
          &msl_info->image[n]->exception);
        if (resample_image == (Image *) NULL)
          break;
        msl_info->image[n]=DestroyImage(msl_info->image[n]);
        msl_info->image[n]=resample_image;
      }
      break;
    }
      if (LocaleCompare((const char *) tag,"resize") == 0)
        {
          double
            blur;

          FilterTypes
            filter;

          Image
            *resize_image;

          /*
            Resize image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          filter=UndefinedFilter;
          blur=1.0;
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'F':
                case 'f':
                {
                  if (LocaleCompare(keyword,"filter") == 0)
                    {
                      option=ParseCommandOption(MagickFilterOptions,MagickFalse,
                        value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedNoiseType",
                          value);
                      filter=(FilterTypes) option;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"geometry") == 0)
                    {
                      flags=ParseRegionGeometry(msl_info->image[n],value,
                        &geometry,exception);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'H':
                case 'h':
                {
                  if (LocaleCompare(keyword,"height") == 0)
                    {
                      geometry.height=StringToUnsignedLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'S':
                case 's':
                {
                  if (LocaleCompare(keyword,"support") == 0)
                    {
                      blur=StringToDouble(value,(char **) NULL);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'W':
                case 'w':
                {
                  if (LocaleCompare(keyword,"width") == 0)
                    {
                      geometry.width=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          resize_image=ResizeImage(msl_info->image[n],geometry.width,
            geometry.height,filter,blur,&msl_info->image[n]->exception);
          if (resize_image == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=resize_image;
          break;
        }
      if (LocaleCompare((const char *) tag,"roll") == 0)
        {
          Image
            *roll_image;

          /*
            Roll image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          SetGeometry(msl_info->image[n],&geometry);
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"geometry") == 0)
                    {
                      flags=ParsePageGeometry(msl_info->image[n],value,
                        &geometry,exception);
                      if ((flags & HeightValue) == 0)
                        geometry.height=geometry.width;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'X':
                case 'x':
                {
                  if (LocaleCompare(keyword,"x") == 0)
                    {
                      geometry.x=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'Y':
                case 'y':
                {
                  if (LocaleCompare(keyword,"y") == 0)
                    {
                      geometry.y=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          roll_image=RollImage(msl_info->image[n],geometry.x,geometry.y,
            &msl_info->image[n]->exception);
          if (roll_image == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=roll_image;
          break;
        }
      else if (LocaleCompare((const char *) tag,"roll") == 0)
      {
        /* init the values */
        width=msl_info->image[n]->columns;
        height=msl_info->image[n]->rows;
        x = y = 0;

        if (msl_info->image[n] == (Image *) NULL)
        {
          ThrowMSLException(OptionError,"NoImagesDefined",
            (const char *) tag);
          break;
        }
        if (attributes == (const xmlChar **) NULL)
        break;
        for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
        {
          keyword=(const char *) attributes[i++];
          attribute=InterpretImageProperties(msl_info->image_info[n],
            msl_info->attributes[n],(const char *) attributes[i]);
          CloneString(&value,attribute);
          attribute=DestroyString(attribute);
        switch (*keyword)
        {
          case 'G':
          case 'g':
          {
          if (LocaleCompare(keyword,"geometry") == 0)
            {
            (void) ParseMetaGeometry(value,&x,&y,&width,&height);
            break;
            }
          ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
          break;
          }
          case 'X':
          case 'x':
          {
          if (LocaleCompare(keyword,"x") == 0)
            {
            x = StringToLong( value );
            break;
            }
          ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
          break;
          }
          case 'Y':
          case 'y':
          {
          if (LocaleCompare(keyword,"y") == 0)
            {
            y = StringToLong( value );
            break;
            }
          ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
          break;
          }
          default:
          {
          ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
          break;
          }
        }
        }

        /*
          process image.
        */
        {
        Image
          *newImage;

        newImage=RollImage(msl_info->image[n], x, y, &msl_info->image[n]->exception);
        if (newImage == (Image *) NULL)
          break;
        msl_info->image[n]=DestroyImage(msl_info->image[n]);
        msl_info->image[n]=newImage;
        }

        break;
      }
      if (LocaleCompare((const char *) tag,"rotate") == 0)
        {
          Image
            *rotate_image;

          /*
            Rotate image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'D':
                case 'd':
                {
                  if (LocaleCompare(keyword,"degrees") == 0)
                    {
                      geometry_info.rho=StringToDouble(value,
                        (char **) NULL);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"geometry") == 0)
                    {
                      flags=ParseGeometry(value,&geometry_info);
                      if ((flags & SigmaValue) == 0)
                        geometry_info.sigma=1.0;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          rotate_image=RotateImage(msl_info->image[n],geometry_info.rho,
            &msl_info->image[n]->exception);
          if (rotate_image == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=rotate_image;
          break;
        }
      else if (LocaleCompare((const char *) tag,"rotate") == 0)
      {
        /* init the values */
        double  degrees = 0;

        if (msl_info->image[n] == (Image *) NULL)
        {
          ThrowMSLException(OptionError,"NoImagesDefined",
            (const char *) tag);
          break;
        }
        if (attributes == (const xmlChar **) NULL)
          break;
        for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
        {
        keyword=(const char *) attributes[i++];
        attribute=InterpretImageProperties(msl_info->image_info[n],
          msl_info->attributes[n],(const char *) attributes[i]);
        CloneString(&value,attribute);
        attribute=DestroyString(attribute);
        switch (*keyword)
        {
          case 'D':
          case 'd':
          {
          if (LocaleCompare(keyword,"degrees") == 0)
            {
            degrees = StringToDouble(value,(char **) NULL);
            break;
            }
          ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
          break;
          }
          default:
          {
          ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
          break;
          }
        }
        }

        /*
          process image.
        */
        {
        Image
          *newImage;

        newImage=RotateImage(msl_info->image[n], degrees, &msl_info->image[n]->exception);
        if (newImage == (Image *) NULL)
          break;
        msl_info->image[n]=DestroyImage(msl_info->image[n]);
        msl_info->image[n]=newImage;
        }

        break;
      }
      ThrowMSLException(OptionError,"UnrecognizedElement",(const char *) tag);
    }
    case 'S':
    case 's':
    {
      if (LocaleCompare((const char *) tag,"sample") == 0)
        {
          Image
            *sample_image;

          /*
            Sample image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"geometry") == 0)
                    {
                      flags=ParseRegionGeometry(msl_info->image[n],value,
                        &geometry,exception);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'H':
                case 'h':
                {
                  if (LocaleCompare(keyword,"height") == 0)
                    {
                      geometry.height=StringToUnsignedLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'W':
                case 'w':
                {
                  if (LocaleCompare(keyword,"width") == 0)
                    {
                      geometry.width=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          sample_image=SampleImage(msl_info->image[n],geometry.width,
            geometry.height,&msl_info->image[n]->exception);
          if (sample_image == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=sample_image;
          break;
        }
      if (LocaleCompare((const char *) tag,"scale") == 0)
        {
          Image
            *scale_image;

          /*
            Scale image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"geometry") == 0)
                    {
                      flags=ParseRegionGeometry(msl_info->image[n],value,
                        &geometry,exception);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'H':
                case 'h':
                {
                  if (LocaleCompare(keyword,"height") == 0)
                    {
                      geometry.height=StringToUnsignedLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'W':
                case 'w':
                {
                  if (LocaleCompare(keyword,"width") == 0)
                    {
                      geometry.width=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          scale_image=ScaleImage(msl_info->image[n],geometry.width,
            geometry.height,&msl_info->image[n]->exception);
          if (scale_image == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=scale_image;
          break;
        }
      if (LocaleCompare((const char *) tag,"segment") == 0)
        {
          ColorspaceType
            colorspace;

          MagickBooleanType
            verbose;

          /*
            Segment image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          geometry_info.rho=1.0;
          geometry_info.sigma=1.5;
          colorspace=sRGBColorspace;
          verbose=MagickFalse;
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'C':
                case 'c':
                {
                  if (LocaleCompare(keyword,"cluster-threshold") == 0)
                    {
                      geometry_info.rho=StringToDouble(value,
                        (char **) NULL);
                      break;
                    }
                  if (LocaleCompare(keyword,"colorspace") == 0)
                    {
                      option=ParseCommandOption(MagickColorspaceOptions,
                        MagickFalse,value);
                      if (option < 0)
                        ThrowMSLException(OptionError,
                          "UnrecognizedColorspaceType",value);
                      colorspace=(ColorspaceType) option;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"geometry") == 0)
                    {
                      flags=ParseGeometry(value,&geometry_info);
                      if ((flags & SigmaValue) == 0)
                        geometry_info.sigma=1.5;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'S':
                case 's':
                {
                  if (LocaleCompare(keyword,"smoothing-threshold") == 0)
                    {
                      geometry_info.sigma=StringToDouble(value,
                        (char **) NULL);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          (void) SegmentImage(msl_info->image[n],colorspace,verbose,
            geometry_info.rho,geometry_info.sigma);
          break;
        }
      else if (LocaleCompare((const char *) tag, "set") == 0)
      {
        if (msl_info->image[n] == (Image *) NULL)
        {
          ThrowMSLException(OptionError,"NoImagesDefined",(const char *) tag);
          break;
        }

        if (attributes == (const xmlChar **) NULL)
          break;
        for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
        {
          keyword=(const char *) attributes[i++];
          attribute=InterpretImageProperties(msl_info->image_info[n],
            msl_info->attributes[n],(const char *) attributes[i]);
          CloneString(&value,attribute);
          attribute=DestroyString(attribute);
          switch (*keyword)
          {
            case 'C':
            case 'c':
            {
              if (LocaleCompare(keyword,"clip-mask") == 0)
                {
                  for (j=0; j < msl_info->n; j++)
                  {
                    const char
                      *property;

                    property=GetImageProperty(msl_info->attributes[j],"id");
                    if (LocaleCompare(property,value) == 0)
                      {
                        SetImageMask(msl_info->image[n],msl_info->image[j]);
                        break;
                      }
                  }
                  break;
                }
              if (LocaleCompare(keyword,"clip-path") == 0)
                {
                  for (j=0; j < msl_info->n; j++)
                  {
                    const char
                      *property;

                    property=GetImageProperty(msl_info->attributes[j],"id");
                    if (LocaleCompare(property,value) == 0)
                      {
                        SetImageClipMask(msl_info->image[n],msl_info->image[j]);
                        break;
                      }
                  }
                  break;
                }
              if (LocaleCompare(keyword,"colorspace") == 0)
                {
                  ssize_t
                    colorspace;

                  colorspace=(ColorspaceType) ParseCommandOption(
                    MagickColorspaceOptions,MagickFalse,value);
                  if (colorspace < 0)
                    ThrowMSLException(OptionError,"UnrecognizedColorspace",
                      value);
                  (void) TransformImageColorspace(msl_info->image[n],
                    (ColorspaceType) colorspace);
                  break;
                }
              (void) SetMSLAttributes(msl_info,keyword,value);
              (void) SetImageProperty(msl_info->image[n],keyword,value);
              break;
            }
            case 'D':
            case 'd':
            {
              if (LocaleCompare(keyword,"density") == 0)
                {
                  flags=ParseGeometry(value,&geometry_info);
                  msl_info->image[n]->x_resolution=geometry_info.rho;
                  msl_info->image[n]->y_resolution=geometry_info.sigma;
                  if ((flags & SigmaValue) == 0)
                    msl_info->image[n]->y_resolution=
                      msl_info->image[n]->x_resolution;
                  break;
                }
              (void) SetMSLAttributes(msl_info,keyword,value);
              (void) SetImageProperty(msl_info->image[n],keyword,value);
              break;
            }
            case 'O':
            case 'o':
            {
              if (LocaleCompare(keyword, "opacity") == 0)
                {
                  ssize_t  opac = OpaqueOpacity,
                  len = (ssize_t) strlen( value );

                  if (value[len-1] == '%') {
                    char  tmp[100];
                    (void) CopyMagickString(tmp,value,len);
                    opac = StringToLong( tmp );
                    opac = (int)(QuantumRange * ((float)opac/100));
                  } else
                    opac = StringToLong( value );
                  (void) SetImageOpacity( msl_info->image[n], (Quantum) opac );
                  break;
              }
              (void) SetMSLAttributes(msl_info,keyword,value);
              (void) SetImageProperty(msl_info->image[n],keyword,value);
              break;
            }
            case 'P':
            case 'p':
            {
              if (LocaleCompare(keyword, "page") == 0)
              {
                char
                  page[MaxTextExtent];

                const char
                  *image_option;

                MagickStatusType
                  flags;

                RectangleInfo
                  geometry;

                (void) memset(&geometry,0,sizeof(geometry));
                image_option=GetImageArtifact(msl_info->image[n],"page");
                if (image_option != (const char *) NULL)
                  flags=ParseAbsoluteGeometry(image_option,&geometry);
                flags=ParseAbsoluteGeometry(value,&geometry);
                (void) FormatLocaleString(page,MaxTextExtent,"%.20gx%.20g",
                  (double) geometry.width,(double) geometry.height);
                if (((flags & XValue) != 0) || ((flags & YValue) != 0))
                  (void) FormatLocaleString(page,MaxTextExtent,
                    "%.20gx%.20g%+.20g%+.20g",(double) geometry.width,
                    (double) geometry.height,(double) geometry.x,(double)
                    geometry.y);
                (void) SetImageOption(msl_info->image_info[n],keyword,page);
                msl_info->image_info[n]->page=GetPageGeometry(page);
                break;
              }
              (void) SetMSLAttributes(msl_info,keyword,value);
              (void) SetImageProperty(msl_info->image[n],keyword,value);
              break;
            }
            default:
            {
              (void) SetMSLAttributes(msl_info,keyword,value);
              (void) SetImageProperty(msl_info->image[n],keyword,value);
              break;
            }
          }
        }
        break;
      }
      if (LocaleCompare((const char *) tag,"shade") == 0)
        {
          Image
            *shade_image;

          MagickBooleanType
            gray;

          /*
            Shade image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          gray=MagickFalse;
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'A':
                case 'a':
                {
                  if (LocaleCompare(keyword,"azimuth") == 0)
                    {
                      geometry_info.rho=StringToDouble(value,
                        (char **) NULL);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'E':
                case 'e':
                {
                  if (LocaleCompare(keyword,"elevation") == 0)
                    {
                      geometry_info.sigma=StringToDouble(value,
                        (char **) NULL);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"geometry") == 0)
                    {
                      flags=ParseGeometry(value,&geometry_info);
                      if ((flags & SigmaValue) == 0)
                        geometry_info.sigma=1.0;
                      break;
                    }
                  if (LocaleCompare(keyword,"gray") == 0)
                    {
                      option=ParseCommandOption(MagickBooleanOptions,MagickFalse,
                        value);
                      if (option < 0)
                        ThrowMSLException(OptionError,"UnrecognizedNoiseType",
                          value);
                      gray=(MagickBooleanType) option;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          shade_image=ShadeImage(msl_info->image[n],gray,geometry_info.rho,
            geometry_info.sigma,&msl_info->image[n]->exception);
          if (shade_image == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=shade_image;
          break;
        }
      if (LocaleCompare((const char *) tag,"shadow") == 0)
        {
          Image
            *shadow_image;

          /*
            Shear image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"geometry") == 0)
                    {
                      flags=ParseGeometry(value,&geometry_info);
                      if ((flags & SigmaValue) == 0)
                        geometry_info.sigma=1.0;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'O':
                case 'o':
                {
                  if (LocaleCompare(keyword,"opacity") == 0)
                    {
                      geometry_info.rho=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'S':
                case 's':
                {
                  if (LocaleCompare(keyword,"sigma") == 0)
                    {
                      geometry_info.sigma=StringToLong(value);
                      break;
                    }
                  break;
                }
                case 'X':
                case 'x':
                {
                  if (LocaleCompare(keyword,"x") == 0)
                    {
                      geometry_info.xi=StringToDouble(value,
                        (char **) NULL);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'Y':
                case 'y':
                {
                  if (LocaleCompare(keyword,"y") == 0)
                    {
                      geometry_info.psi=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          shadow_image=ShadowImage(msl_info->image[n],geometry_info.rho,
            geometry_info.sigma,(ssize_t) ceil(geometry_info.xi-0.5),(ssize_t)
            ceil(geometry_info.psi-0.5),&msl_info->image[n]->exception);
          if (shadow_image == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=shadow_image;
          break;
        }
      if (LocaleCompare((const char *) tag,"sharpen") == 0)
      {
        double  radius = 0.0,
            sigma = 1.0;

        if (msl_info->image[n] == (Image *) NULL)
          {
            ThrowMSLException(OptionError,"NoImagesDefined",
              (const char *) tag);
            break;
          }
        /*
        NOTE: sharpen can have no attributes, since we use all the defaults!
        */
        if (attributes != (const xmlChar **) NULL)
        {
          for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
          {
          keyword=(const char *) attributes[i++];
          attribute=InterpretImageProperties(msl_info->image_info[n],
            msl_info->attributes[n],(const char *) attributes[i]);
          CloneString(&value,attribute);
          attribute=DestroyString(attribute);
          switch (*keyword)
          {
            case 'R':
            case 'r':
            {
              if (LocaleCompare(keyword, "radius") == 0)
              {
                radius = StringToDouble(value,(char **) NULL);
                break;
              }
              ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
              break;
            }
            case 'S':
            case 's':
            {
              if (LocaleCompare(keyword,"sigma") == 0)
              {
                sigma = StringToLong( value );
                break;
              }
              ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
              break;
            }
            default:
            {
              ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
              break;
            }
          }
          }
        }

        /*
          sharpen image.
        */
        {
        Image
          *newImage;

        newImage=SharpenImage(msl_info->image[n],radius,sigma,&msl_info->image[n]->exception);
        if (newImage == (Image *) NULL)
          break;
        msl_info->image[n]=DestroyImage(msl_info->image[n]);
        msl_info->image[n]=newImage;
        break;
        }
      }
      else if (LocaleCompare((const char *) tag,"shave") == 0)
      {
        /* init the values */
        width = height = 0;
        x = y = 0;

        if (msl_info->image[n] == (Image *) NULL)
        {
          ThrowMSLException(OptionError,"NoImagesDefined",
            (const char *) tag);
          break;
        }
        if (attributes == (const xmlChar **) NULL)
        break;
        for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
        {
          keyword=(const char *) attributes[i++];
          attribute=InterpretImageProperties(msl_info->image_info[n],
            msl_info->attributes[n],(const char *) attributes[i]);
          CloneString(&value,attribute);
          attribute=DestroyString(attribute);
        switch (*keyword)
        {
          case 'G':
          case 'g':
          {
          if (LocaleCompare(keyword,"geometry") == 0)
            {
            (void) ParseMetaGeometry(value,&x,&y,&width,&height);
            break;
            }
          ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
          break;
          }
          case 'H':
          case 'h':
          {
          if (LocaleCompare(keyword,"height") == 0)
            {
            height = StringToLong( value );
            break;
            }
          ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
          break;
          }
          case 'W':
          case 'w':
          {
          if (LocaleCompare(keyword,"width") == 0)
            {
            width = StringToLong( value );
            break;
            }
          ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
          break;
          }
          default:
          {
          ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
          break;
          }
        }
        }

        /*
          process image.
        */
        {
        Image
          *newImage;
        RectangleInfo
          rectInfo;

        rectInfo.height = height;
        rectInfo.width = width;
        rectInfo.x = x;
        rectInfo.y = y;


        newImage=ShaveImage(msl_info->image[n], &rectInfo,
          &msl_info->image[n]->exception);
        if (newImage == (Image *) NULL)
          break;
        msl_info->image[n]=DestroyImage(msl_info->image[n]);
        msl_info->image[n]=newImage;
        }

        break;
      }
      if (LocaleCompare((const char *) tag,"shear") == 0)
        {
          Image
            *shear_image;

          /*
            Shear image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'F':
                case 'f':
                {
                  if (LocaleCompare(keyword, "fill") == 0)
                    {
                      (void) QueryColorDatabase(value,
                        &msl_info->image[n]->background_color,exception);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"geometry") == 0)
                    {
                      flags=ParseGeometry(value,&geometry_info);
                      if ((flags & SigmaValue) == 0)
                        geometry_info.sigma=1.0;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'X':
                case 'x':
                {
                  if (LocaleCompare(keyword,"x") == 0)
                    {
                      geometry_info.rho=StringToDouble(value,
                        (char **) NULL);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'Y':
                case 'y':
                {
                  if (LocaleCompare(keyword,"y") == 0)
                    {
                      geometry_info.sigma=StringToLong(value);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          shear_image=ShearImage(msl_info->image[n],geometry_info.rho,
            geometry_info.sigma,&msl_info->image[n]->exception);
          if (shear_image == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=shear_image;
          break;
        }
      if (LocaleCompare((const char *) tag,"signature") == 0)
        {
          /*
            Signature image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          (void) SignatureImage(msl_info->image[n]);
          break;
        }
      if (LocaleCompare((const char *) tag,"solarize") == 0)
        {
          /*
            Solarize image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          geometry_info.rho=QuantumRange/2.0;
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"geometry") == 0)
                    {
                      flags=ParseGeometry(value,&geometry_info);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'T':
                case 't':
                {
                  if (LocaleCompare(keyword,"threshold") == 0)
                    {
                      geometry_info.rho=StringToDouble(value,
                        (char **) NULL);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          (void) SolarizeImage(msl_info->image[n],geometry_info.rho);
          break;
        }
      if (LocaleCompare((const char *) tag,"spread") == 0)
        {
          Image
            *spread_image;

          /*
            Spread image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"geometry") == 0)
                    {
                      flags=ParseGeometry(value,&geometry_info);
                      if ((flags & SigmaValue) == 0)
                        geometry_info.sigma=1.0;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'R':
                case 'r':
                {
                  if (LocaleCompare(keyword,"radius") == 0)
                    {
                      geometry_info.rho=StringToDouble(value,
                        (char **) NULL);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          spread_image=SpreadImage(msl_info->image[n],geometry_info.rho,
            &msl_info->image[n]->exception);
          if (spread_image == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=spread_image;
          break;
        }
      else if (LocaleCompare((const char *) tag,"stegano") == 0)
      {
        Image *
          watermark = (Image*) NULL;

        if (msl_info->image[n] == (Image *) NULL)
          {
            ThrowMSLException(OptionError,"NoImagesDefined",
              (const char *) tag);
            break;
          }
        if (attributes == (const xmlChar **) NULL)
        break;
        for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
        {
          keyword=(const char *) attributes[i++];
          attribute=InterpretImageProperties(msl_info->image_info[n],
            msl_info->attributes[n],(const char *) attributes[i]);
          CloneString(&value,attribute);
          attribute=DestroyString(attribute);
        switch (*keyword)
        {
          case 'I':
          case 'i':
          {
          if (LocaleCompare(keyword,"image") == 0)
            {
            for (j=0; j<msl_info->n;j++)
            {
              const char *
                theAttr = GetImageProperty(msl_info->attributes[j], "id");
              if (theAttr && LocaleCompare(theAttr, value) == 0)
              {
                watermark = msl_info->image[j];
                break;
              }
            }
            break;
            }
          ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
          break;
          }
          default:
          {
          ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
          break;
          }
        }
        }

        /*
          process image.
        */
        if ( watermark != (Image*) NULL )
        {
        Image
          *newImage;

        newImage=SteganoImage(msl_info->image[n], watermark, &msl_info->image[n]->exception);
        if (newImage == (Image *) NULL)
          break;
        msl_info->image[n]=DestroyImage(msl_info->image[n]);
        msl_info->image[n]=newImage;
        break;
        } else
          ThrowMSLException(OptionError,"MissingWatermarkImage",keyword);
      }
      else if (LocaleCompare((const char *) tag,"stereo") == 0)
      {
        Image *
          stereoImage = (Image*) NULL;

        if (msl_info->image[n] == (Image *) NULL)
          {
            ThrowMSLException(OptionError,"NoImagesDefined",(const char *) tag);
            break;
          }
        if (attributes == (const xmlChar **) NULL)
        break;
        for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
        {
          keyword=(const char *) attributes[i++];
          attribute=InterpretImageProperties(msl_info->image_info[n],
            msl_info->attributes[n],(const char *) attributes[i]);
          CloneString(&value,attribute);
          attribute=DestroyString(attribute);
        switch (*keyword)
        {
          case 'I':
          case 'i':
          {
          if (LocaleCompare(keyword,"image") == 0)
            {
            for (j=0; j<msl_info->n;j++)
            {
              const char *
                theAttr = GetImageProperty(msl_info->attributes[j], "id");
              if (theAttr && LocaleCompare(theAttr, value) == 0)
              {
                stereoImage = msl_info->image[j];
                break;
              }
            }
            break;
            }
          ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
          break;
          }
          default:
          {
          ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
          break;
          }
        }
        }

        /*
          process image.
        */
        if ( stereoImage != (Image*) NULL )
        {
        Image
          *newImage;

        newImage=StereoImage(msl_info->image[n], stereoImage, &msl_info->image[n]->exception);
        if (newImage == (Image *) NULL)
          break;
        msl_info->image[n]=DestroyImage(msl_info->image[n]);
        msl_info->image[n]=newImage;
        break;
        } else
          ThrowMSLException(OptionError,"Missing stereo image",keyword);
      }
      if (LocaleCompare((const char *) tag,"strip") == 0)
        {
          /*
            Strip image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
            }
          (void) StripImage(msl_info->image[n]);
          break;
        }
      if (LocaleCompare((const char *) tag,"swap") == 0)
        {
          Image
            *p,
            *q,
            *swap;

          ssize_t
            index,
            swap_index;

          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          index=(-1);
          swap_index=(-2);
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"indexes") == 0)
                    {
                      flags=ParseGeometry(value,&geometry_info);
                      index=(ssize_t) geometry_info.rho;
                      if ((flags & SigmaValue) == 0)
                        swap_index=(ssize_t) geometry_info.sigma;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          /*
            Swap images.
          */
          p=GetImageFromList(msl_info->image[n],index);
          q=GetImageFromList(msl_info->image[n],swap_index);
          if ((p == (Image *) NULL) || (q == (Image *) NULL))
            {
              ThrowMSLException(OptionError,"NoSuchImage",(const char *) tag);
              break;
            }
          swap=CloneImage(p,0,0,MagickTrue,&p->exception);
          ReplaceImageInList(&p,CloneImage(q,0,0,MagickTrue,&q->exception));
          ReplaceImageInList(&q,swap);
          msl_info->image[n]=GetFirstImageInList(q);
          break;
        }
      if (LocaleCompare((const char *) tag,"swirl") == 0)
        {
          Image
            *swirl_image;

          /*
            Swirl image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'D':
                case 'd':
                {
                  if (LocaleCompare(keyword,"degrees") == 0)
                    {
                      geometry_info.rho=StringToDouble(value,
                        (char **) NULL);
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                case 'G':
                case 'g':
                {
                  if (LocaleCompare(keyword,"geometry") == 0)
                    {
                      flags=ParseGeometry(value,&geometry_info);
                      if ((flags & SigmaValue) == 0)
                        geometry_info.sigma=1.0;
                      break;
                    }
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          swirl_image=SwirlImage(msl_info->image[n],geometry_info.rho,
            &msl_info->image[n]->exception);
          if (swirl_image == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=swirl_image;
          break;
        }
      if (LocaleCompare((const char *) tag,"sync") == 0)
        {
          /*
            Sync image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          (void) SyncImage(msl_info->image[n]);
          break;
        }
      ThrowMSLException(OptionError,"UnrecognizedElement",(const char *) tag);
    }
    case 'T':
    case 't':
    {
      if (LocaleCompare((const char *) tag,"map") == 0)
        {
          Image
            *texture_image;

          /*
            Texture image.
          */
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          texture_image=NewImageList();
          if (attributes != (const xmlChar **) NULL)
            for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
            {
              keyword=(const char *) attributes[i++];
              attribute=InterpretImageProperties(msl_info->image_info[n],
                msl_info->attributes[n],(const char *) attributes[i]);
              CloneString(&value,attribute);
              attribute=DestroyString(attribute);
              switch (*keyword)
              {
                case 'I':
                case 'i':
                {
                  if (LocaleCompare(keyword,"image") == 0)
                    for (j=0; j < msl_info->n; j++)
                    {
                      const char
                        *attribute;

                      attribute=GetImageProperty(msl_info->attributes[j],"id");
                      if ((attribute != (const char *) NULL)  &&
                          (LocaleCompare(attribute,value) == 0))
                        {
                          texture_image=CloneImage(msl_info->image[j],0,0,
                            MagickFalse,exception);
                          break;
                        }
                    }
                  break;
                }
                default:
                {
                  ThrowMSLException(OptionError,"UnrecognizedAttribute",
                    keyword);
                  break;
                }
              }
            }
          (void) TextureImage(msl_info->image[n],texture_image);
          texture_image=DestroyImage(texture_image);
          break;
        }
      else if (LocaleCompare((const char *) tag,"threshold") == 0)
      {
        /* init the values */
        double  threshold = 0;

        if (msl_info->image[n] == (Image *) NULL)
          {
            ThrowMSLException(OptionError,"NoImagesDefined",(const char *) tag);
            break;
          }
        if (attributes == (const xmlChar **) NULL)
        break;
        for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
        {
          keyword=(const char *) attributes[i++];
          attribute=InterpretImageProperties(msl_info->image_info[n],
            msl_info->attributes[n],(const char *) attributes[i]);
          CloneString(&value,attribute);
          attribute=DestroyString(attribute);
        switch (*keyword)
        {
          case 'T':
          case 't':
          {
          if (LocaleCompare(keyword,"threshold") == 0)
            {
            threshold = StringToDouble(value,(char **) NULL);
            break;
            }
          ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
          break;
          }
          default:
          {
          ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
          break;
          }
        }
        }

        /*
          process image.
        */
        {
        BilevelImageChannel(msl_info->image[n],
          (ChannelType) ((ssize_t) (CompositeChannels &~ (ssize_t) OpacityChannel)),
          threshold);
        break;
        }
      }
      else if (LocaleCompare((const char *) tag, "transparent") == 0)
      {
        if (msl_info->image[n] == (Image *) NULL)
          {
            ThrowMSLException(OptionError,"NoImagesDefined",(const char *) tag);
            break;
          }
        if (attributes == (const xmlChar **) NULL)
          break;
        for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
        {
          keyword=(const char *) attributes[i++];
          attribute=InterpretImageProperties(msl_info->image_info[n],
            msl_info->attributes[n],(const char *) attributes[i]);
          CloneString(&value,attribute);
          attribute=DestroyString(attribute);
          switch (*keyword)
          {
            case 'C':
            case 'c':
            {
              if (LocaleCompare(keyword,"color") == 0)
              {
                MagickPixelPacket
                  target;

                (void) QueryMagickColor(value,&target,exception);
                (void) TransparentPaintImage(msl_info->image[n],&target,
                  TransparentOpacity,MagickFalse);
                break;
              }
              ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
              break;
            }
            default:
            {
              ThrowMSLException(OptionError,"UnrecognizedAttribute",keyword);
            break;
            }
          }
        }
        break;
      }
      else if (LocaleCompare((const char *) tag, "trim") == 0)
      {
        if (msl_info->image[n] == (Image *) NULL)
          {
            ThrowMSLException(OptionError,"NoImagesDefined",(const char *) tag);
            break;
          }

        /* no attributes here */

        /* process the image */
        {
          Image
            *newImage;
          RectangleInfo
            rectInfo;

          /* all zeros on a crop == trim edges! */
          rectInfo.height = rectInfo.width = 0;
          rectInfo.x =  rectInfo.y = 0;

          newImage=CropImage(msl_info->image[n],&rectInfo, &msl_info->image[n]->exception);
          if (newImage == (Image *) NULL)
            break;
          msl_info->image[n]=DestroyImage(msl_info->image[n]);
          msl_info->image[n]=newImage;
          break;
        }
      }
      ThrowMSLException(OptionError,"UnrecognizedElement",(const char *) tag);
    }
    case 'W':
    case 'w':
    {
      if (LocaleCompare((const char *) tag,"write") == 0)
        {
          if (msl_info->image[n] == (Image *) NULL)
            {
              ThrowMSLException(OptionError,"NoImagesDefined",
                (const char *) tag);
              break;
            }
          if (attributes == (const xmlChar **) NULL)
            break;
          for (i=0; (attributes[i] != (const xmlChar *) NULL); i++)
          {
            keyword=(const char *) attributes[i++];
            attribute=InterpretImageProperties(msl_info->image_info[n],
              msl_info->attributes[n],(const char *) attributes[i]);
            CloneString(&value,attribute);
            attribute=DestroyString(attribute);
            switch (*keyword)
            {
              case 'F':
              case 'f':
              {
                if (LocaleCompare(keyword,"filename") == 0)
                  {
                    (void) CopyMagickString(msl_info->image[n]->filename,value,
                      MaxTextExtent);
                    break;
                  }
                (void) SetMSLAttributes(msl_info,keyword,value);
              }
              default:
              {
                (void) SetMSLAttributes(msl_info,keyword,value);
                break;
              }
            }
          }

          /* process */
          {
            *msl_info->image_info[n]->magick='\0';
            (void) WriteImage(msl_info->image_info[n], msl_info->image[n]);
            break;
          }
        }
      ThrowMSLException(OptionError,"UnrecognizedElement",(const char *) tag);
    }
    default:
    {
      ThrowMSLException(OptionError,"UnrecognizedElement",(const char *) tag);
      break;
    }
  }
  if (value != (char *) NULL)
    value=DestroyString(value);
  exception=DestroyExceptionInfo(exception);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"  )");
}