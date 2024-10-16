MagickExport MagickBooleanType IsCommandOption(const char *option)
{
  assert(option != (const char *) NULL);
  if ((*option != '-') && (*option != '+'))
    return(MagickFalse);
  if (strlen(option) == 1)
    return(((*option == '{') || (*option == '}') || (*option == '[') ||
      (*option == ']')) ? MagickTrue : MagickFalse);
  option++;
  if (*option == '-')
    return(MagickTrue);
  if (isalpha((int) ((unsigned char) *option)) == 0)
    return(MagickFalse);
  return(MagickTrue);
}