MagickExport char *DestroyString(char *string)
{
  return((char *) RelinquishMagickMemory(string));
}