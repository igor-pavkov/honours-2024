MagickExport Image *ChannelFxImage(const Image *image,const char *expression,
  ExceptionInfo *exception)
{
#define ChannelFxImageTag  "ChannelFx/Image"

  ChannelFx
    channel_op;

  ChannelType
    channel_mask;

  char
    token[MagickPathExtent];

  const char
    *p;

  const Image
    *source_image;

  double
    pixel;

  Image
    *destination_image;

  MagickBooleanType
    status;

  PixelChannel
    source_channel,
    destination_channel;

  ssize_t
    channels;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  source_image=image;
  destination_image=CloneImage(source_image,0,0,MagickTrue,exception);
  if (destination_image == (Image *) NULL)
    return((Image *) NULL);
  if (expression == (const char *) NULL)
    return(destination_image);
  status=SetImageStorageClass(destination_image,DirectClass,exception);
  if (status == MagickFalse)
    {
      destination_image=GetLastImageInList(destination_image);
      return((Image *) NULL);
    }
  destination_channel=RedPixelChannel;
  channel_mask=UndefinedChannel;
  pixel=0.0;
  p=(char *) expression;
  (void) GetNextToken(p,&p,MagickPathExtent,token);
  channel_op=ExtractChannelOp;
  for (channels=0; *token != '\0'; )
  {
    ssize_t
      i;

    /*
      Interpret channel expression.
    */
    switch (*token)
    {
      case ',':
      {
        (void) GetNextToken(p,&p,MagickPathExtent,token);
        break;
      }
      case '|':
      {
        if (GetNextImageInList(source_image) != (Image *) NULL)
          source_image=GetNextImageInList(source_image);
        else
          source_image=GetFirstImageInList(source_image);
        (void) GetNextToken(p,&p,MagickPathExtent,token);
        break;
      }
      case ';':
      {
        Image
          *canvas;

        (void) SetPixelChannelMask(destination_image,channel_mask);
        if ((channel_op == ExtractChannelOp) && (channels == 1))
          {
            (void) SetPixelMetaChannels(destination_image,0,exception);
            (void) SetImageColorspace(destination_image,GRAYColorspace,
              exception);
          }
        canvas=CloneImage(source_image,0,0,MagickTrue,exception);
        if (canvas == (Image *) NULL)
          {
            destination_image=DestroyImageList(destination_image);
            return(destination_image);
          }
        AppendImageToList(&destination_image,canvas);
        destination_image=GetLastImageInList(destination_image);
        status=SetImageStorageClass(destination_image,DirectClass,exception);
        if (status == MagickFalse)
          {
            destination_image=GetLastImageInList(destination_image);
            return((Image *) NULL);
          }
        (void) GetNextToken(p,&p,MagickPathExtent,token);
        channels=0;
        destination_channel=RedPixelChannel;
        channel_mask=UndefinedChannel;
        break;
      }
      default:
        break;
    }
    i=ParsePixelChannelOption(token);
    if (i < 0)
      {
        (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
          "UnrecognizedChannelType","`%s'",token);
        destination_image=DestroyImageList(destination_image);
        return(destination_image);
      }
    source_channel=(PixelChannel) i;
    channel_op=ExtractChannelOp;
    (void) GetNextToken(p,&p,MagickPathExtent,token);
    if (*token == '<')
      {
        channel_op=ExchangeChannelOp;
        (void) GetNextToken(p,&p,MagickPathExtent,token);
      }
    if (*token == '=')
      {
        if (channel_op != ExchangeChannelOp)
          channel_op=AssignChannelOp;
        (void) GetNextToken(p,&p,MagickPathExtent,token);
      }
    if (*token == '>')
      {
        if (channel_op != ExchangeChannelOp)
          channel_op=TransferChannelOp;
        (void) GetNextToken(p,&p,MagickPathExtent,token);
      }
    switch (channel_op)
    {
      case AssignChannelOp:
      case ExchangeChannelOp:
      case TransferChannelOp:
      {
        if (channel_op == AssignChannelOp)
          pixel=StringToDoubleInterval(token,(double) QuantumRange+1.0);
        else
          {
            i=ParsePixelChannelOption(token);
            if (i < 0)
              {
                (void) ThrowMagickException(exception,GetMagickModule(),
                  OptionError,"UnrecognizedChannelType","`%s'",token);
                destination_image=DestroyImageList(destination_image);
                return(destination_image);
              }
          }
        destination_channel=(PixelChannel) i;
        if (i >= (ssize_t) GetPixelChannels(destination_image))
          (void) SetPixelMetaChannels(destination_image,(size_t) (
            destination_channel-GetPixelChannels(destination_image)+1),
            exception);
        if (image->colorspace != UndefinedColorspace)
          switch (destination_channel)
          {
            case RedPixelChannel:
            case GreenPixelChannel:
            case BluePixelChannel:
            case BlackPixelChannel:
            case IndexPixelChannel:
              break;
            case AlphaPixelChannel:
            {
              destination_image->alpha_trait=BlendPixelTrait;
              break;
            }
            case CompositeMaskPixelChannel:
            {
              destination_image->channels=(ChannelType)
                (destination_image->channels | CompositeMaskChannel);
              break;
            }
            case ReadMaskPixelChannel:
            {
              destination_image->channels=(ChannelType)
                (destination_image->channels | ReadMaskChannel);
              break;
            }
            case WriteMaskPixelChannel:
            {
              destination_image->channels=(ChannelType)
                (destination_image->channels | WriteMaskChannel);
              break;
            }
            case MetaPixelChannel:
            default:
            {
              (void) SetPixelMetaChannels(destination_image,(size_t) (
                destination_channel-GetPixelChannels(destination_image)+1),
                exception);
              break;
            }
          }
        channel_mask=(ChannelType) (channel_mask | ParseChannelOption(token));
        if (((channels >= 1)  || (destination_channel >= 1)) &&
            (IsGrayColorspace(destination_image->colorspace) != MagickFalse))
          (void) SetImageColorspace(destination_image,sRGBColorspace,exception);
        (void) GetNextToken(p,&p,MagickPathExtent,token);
        break;
      }
      default:
        break;
    }
    status=ChannelImage(destination_image,destination_channel,channel_op,
      source_image,source_channel,ClampToQuantum(pixel),exception);
    if (status == MagickFalse)
      {
        destination_image=DestroyImageList(destination_image);
        break;
      }
    channels++;
    if (channel_op == ExchangeChannelOp)
      {
        status=ChannelImage(destination_image,source_channel,channel_op,
          source_image,destination_channel,ClampToQuantum(pixel),exception);
        if (status == MagickFalse)
          {
            destination_image=DestroyImageList(destination_image);
            break;
          }
        channels++;
      }
    switch (channel_op)
    {
      case ExtractChannelOp:
      {
        channel_mask=(ChannelType) (channel_mask |
          (1UL << destination_channel));
        destination_channel=(PixelChannel) (destination_channel+1);
        break;
      }
      default:
        break;
    }
    status=SetImageProgress(source_image,ChannelFxImageTag,p-expression,
      strlen(expression));
    if (status == MagickFalse)
      break;
  }
  (void) SetPixelChannelMask(destination_image,channel_mask);
  if ((channel_op == ExtractChannelOp) && (channels == 1))
    {
      (void) SetPixelMetaChannels(destination_image,0,exception);
      (void) SetImageColorspace(destination_image,GRAYColorspace,exception);
    }
  return(GetFirstImageInList(destination_image));
}