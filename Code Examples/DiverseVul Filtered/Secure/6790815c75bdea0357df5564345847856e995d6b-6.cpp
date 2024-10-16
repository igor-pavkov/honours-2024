MagickExport char **GetCommandOptions(const CommandOption option)
{
  char
    **options;

  const OptionInfo
    *option_info;

  register ssize_t
    i;

  option_info=GetOptionInfo(option);
  if (option_info == (const OptionInfo *) NULL)
    return((char **) NULL);
  for (i=0; option_info[i].mnemonic != (const char *) NULL; i++) ;
  options=(char **) AcquireQuantumMemory((size_t) i+1UL,sizeof(*options));
  if (options == (char **) NULL)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  for (i=0; option_info[i].mnemonic != (const char *) NULL; i++)
    options[i]=AcquireString(option_info[i].mnemonic);
  options[i]=(char *) NULL;
  return(options);
}