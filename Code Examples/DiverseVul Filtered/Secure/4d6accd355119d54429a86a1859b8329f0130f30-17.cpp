static MngInfo *MngInfoFreeStruct(MngInfo *mng_info)
{
  register ssize_t
    i;

  if (mng_info == (MngInfo *) NULL)
    return((MngInfo *) NULL);

  for (i=1; i < MNG_MAX_OBJECTS; i++)
    MngInfoDiscardObject(mng_info,i);

  mng_info->global_plte=(png_colorp)
    RelinquishMagickMemory(mng_info->global_plte);

  return((MngInfo *) RelinquishMagickMemory(mng_info));
}