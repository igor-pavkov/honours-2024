MagickExport ssize_t GetCommandOptionFlags(const CommandOption option,
  const MagickBooleanType list,const char *options)
{
  char
    token[MagickPathExtent];

  const OptionInfo
    *command_info,
    *option_info;

  int
    sentinel;

  MagickBooleanType
    negate;

  register char
    *q;

  register const char
    *p;

  register ssize_t
    i;

  ssize_t
    option_types;

  option_info=GetOptionInfo(option);
  if (option_info == (const OptionInfo *) NULL)
    return(UndefinedOptionFlag);
  option_types=0;
  sentinel=',';
  if (strchr(options,'|') != (char *) NULL)
    sentinel='|';
  for (p=options; p != (char *) NULL; p=strchr(p,sentinel))
  {
    while (((isspace((int) ((unsigned char) *p)) != 0) || (*p == sentinel)) &&
           (*p != '\0'))
      p++;
    negate=(*p == '!') ? MagickTrue : MagickFalse;
    if (negate != MagickFalse)
      p++;
    q=token;
    while (((isspace((int) ((unsigned char) *p)) == 0) && (*p != sentinel)) &&
           (*p != '\0'))
    {
      if ((q-token) >= (MagickPathExtent-1))
        break;
      *q++=(*p++);
    }
    *q='\0';
    for (i=0; option_info[i].mnemonic != (char *) NULL; i++)
      if (LocaleCompare(token,option_info[i].mnemonic) == 0)
        break;
    command_info=option_info+i;
    if ((command_info->mnemonic == (const char *) NULL) &&
        ((strchr(token+1,'-') != (char *) NULL) ||
         (strchr(token+1,'_') != (char *) NULL)))
      {
        while ((q=strchr(token+1,'-')) != (char *) NULL)
          (void) CopyMagickString(q,q+1,MagickPathExtent-strlen(q));
        while ((q=strchr(token+1,'_')) != (char *) NULL)
          (void) CopyMagickString(q,q+1,MagickPathExtent-strlen(q));
        for (i=0; option_info[i].mnemonic != (char *) NULL; i++)
          if (LocaleCompare(token,option_info[i].mnemonic) == 0)
            break;
        command_info=option_info+i;
      }
    if (command_info->mnemonic == (const char *) NULL)
      return(-1);
    if (negate != MagickFalse)
      option_types=option_types &~ command_info->flags;
    else
      option_types=option_types | command_info->flags;
    if (list == MagickFalse)
      break;
  }
  return(option_types);
}