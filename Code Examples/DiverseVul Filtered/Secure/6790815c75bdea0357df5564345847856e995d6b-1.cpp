MagickExport MagickBooleanType ListCommandOptions(FILE *file,
  const CommandOption option,ExceptionInfo *magick_unused(exception))
{
  const OptionInfo
    *option_info;

  register ssize_t
    i;

  if (file == (FILE *) NULL)
    file=stdout;
  option_info=GetOptionInfo(option);
  if (option_info == (const OptionInfo *) NULL)
    return(MagickFalse);
  for (i=0; option_info[i].mnemonic != (char *) NULL; i++)
  {
    if (option_info[i].stealth != MagickFalse)
      continue;
    (void) FormatLocaleFile(file,"%s\n",option_info[i].mnemonic);
  }
  return(MagickTrue);
}