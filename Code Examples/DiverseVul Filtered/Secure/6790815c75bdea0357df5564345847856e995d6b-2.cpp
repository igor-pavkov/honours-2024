MagickExport const OptionInfo *GetCommandOptionInfo(const char *option)
{
  register ssize_t
    i;

  for (i=0; CommandOptions[i].mnemonic != (char *) NULL; i++)
    if (LocaleCompare(option,CommandOptions[i].mnemonic) == 0)
      break;
  return(CommandOptions+i);
}