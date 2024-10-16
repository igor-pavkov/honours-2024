static void *AcquireCompressionMemory(void *context,const size_t items,
  const size_t size)
{
  size_t
    extent;

  (void) context;
  if (HeapOverflowSanityCheck(items,size) != MagickFalse)
    return((void *) NULL);
  extent=items*size;
  if (extent > GetMaxMemoryRequest())
    return((void *) NULL);
  return(AcquireMagickMemory(extent));
}