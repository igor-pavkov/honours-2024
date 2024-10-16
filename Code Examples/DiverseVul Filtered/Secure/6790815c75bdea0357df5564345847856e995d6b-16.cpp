MagickExport ssize_t ParseChannelOption(const char *channels)
{
  register ssize_t
    i;

  ssize_t
    channel;

  channel=ParseCommandOption(MagickChannelOptions,MagickTrue,channels);
  if (channel >= 0)
    return(channel);
  channel=0;
  for (i=0; i < (ssize_t) strlen(channels); i++)
  {
    switch (channels[i])
    {
      case 'A':
      case 'a':
      {
        channel|=AlphaChannel;
        break;
      }
      case 'B':
      case 'b':
      {
        channel|=BlueChannel;
        break;
      }
      case 'C':
      case 'c':
      {
        channel|=CyanChannel;
        break;
      }
      case 'g':
      case 'G':
      {
        channel|=GreenChannel;
        break;
      }
      case 'K':
      case 'k':
      {
        channel|=BlackChannel;
        break;
      }
      case 'M':
      case 'm':
      {
        channel|=MagentaChannel;
        break;
      }
      case 'o':
      case 'O':
      {
        channel|=AlphaChannel; /* depreciate */
        break;
      }
      case 'R':
      case 'r':
      {
        channel|=RedChannel;
        break;
      }
      case 'Y':
      case 'y':
      {
        channel|=YellowChannel;
        break;
      }
      case ',':
      {
        ssize_t
          type;

        /*
          Gather the additional channel flags and merge with shorthand.
        */
        type=ParseCommandOption(MagickChannelOptions,MagickTrue,channels+i+1);
        if (type < 0)
          return(type);
        channel|=type;
        return(channel);
      }
      default:
        return(-1);
    }
  }
  return(channel);
}