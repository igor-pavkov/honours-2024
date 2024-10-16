static inline void RelinquishDCMMemory(DCMInfo *info,DCMMap *map,
  DCMStreamInfo *stream_info,LinkedListInfo *stack,unsigned char *data)
{
  if (info->scale != (Quantum *) NULL)
    info->scale=(Quantum *) RelinquishMagickMemory(info->scale);
  if (map->gray != (int *) NULL)
    map->gray=(int *) RelinquishMagickMemory(map->gray);
  if (map->blue != (int *) NULL)
    map->blue=(int *) RelinquishMagickMemory(map->blue);
  if (map->green != (int *) NULL)
    map->green=(int *) RelinquishMagickMemory(map->green);
  if (map->red != (int *) NULL)
    map->red=(int *) RelinquishMagickMemory(map->red);
  if (stream_info->offsets != (ssize_t *) NULL)
    stream_info->offsets=(ssize_t *) RelinquishMagickMemory(
      stream_info->offsets);
  if (stream_info != (DCMStreamInfo *) NULL)
    stream_info=(DCMStreamInfo *) RelinquishMagickMemory(stream_info);
  if (stack != (LinkedListInfo *) NULL)
    stack=DestroyLinkedList(stack,RelinquishDCMInfo);
  if (data != (unsigned char *) NULL)
    data=(unsigned char *) RelinquishMagickMemory(data);
}