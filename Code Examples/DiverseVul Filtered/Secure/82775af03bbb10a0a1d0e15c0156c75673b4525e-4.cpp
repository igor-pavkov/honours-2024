static void *RelinquishDCMInfo(void *memory)
{
  DCMInfo
    *info;

  info=(DCMInfo *) memory;
  if (info->scale != (Quantum *) NULL)
    info->scale=(Quantum *) RelinquishMagickMemory(info->scale);
  return(RelinquishMagickMemory(info));
}