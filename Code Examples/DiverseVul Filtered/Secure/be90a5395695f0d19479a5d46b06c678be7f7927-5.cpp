MagickExport char **DestroyStringList(char **list)
{
  register ssize_t
    i;

  assert(list != (char **) NULL);
  for (i=0; list[i] != (char *) NULL; i++)
    list[i]=DestroyString(list[i]);
  list=(char **) RelinquishMagickMemory(list);
  return(list);
}