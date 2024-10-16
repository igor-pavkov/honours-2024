static void *AcquireBZIPMemory(void *context,int items,int size)
{
  return(AcquireCompressionMemory(context,(size_t) items,(size_t) size));
}