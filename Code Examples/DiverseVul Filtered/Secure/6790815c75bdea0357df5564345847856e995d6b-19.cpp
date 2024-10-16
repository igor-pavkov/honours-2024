MagickExport ssize_t ParsePixelChannelOption(const char *channels)
{
  char
    *q,
    token[MagickPathExtent];

  ssize_t
    channel;

  GetMagickToken(channels,NULL,token);
  if ((*token == ';') || (*token == '|'))
    return(RedPixelChannel);
  channel=ParseCommandOption(MagickPixelChannelOptions,MagickTrue,token);
  if (channel >= 0)
    return(channel);
  q=(char *) token;
  channel=(ssize_t) InterpretLocaleValue(token,&q);
  if ((q == token) || (channel < 0) || (channel >= MaxPixelChannels))
    return(-1);
  return(channel);
}