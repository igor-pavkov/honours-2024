static void *AcquireLZMAMemory(void *context,size_t items,size_t size)
{
  return(AcquireCompressionMemory(context,items,size));
}