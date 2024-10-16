MagickPrivate MagickBooleanType IsGlob(const char *path)
{
  MagickBooleanType
    status = MagickFalse;

  register const char
    *p;

  if (IsPathAccessible(path) != MagickFalse)
    return(MagickFalse);
  for (p=path; *p != '\0'; p++)
  {
    switch (*p)
    {
      case '*':
      case '?':
      case '{':
      case '}':
      case '[':
      case ']':
      {
        status=MagickTrue;
        break;
      }
      default:
        break;
    }
  }
  return(status);
}