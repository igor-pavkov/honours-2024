static voidpf AcquireZIPMemory(voidpf context,unsigned int items,
  unsigned int size)
{
  return((voidpf) AcquireCompressionMemory(context,(size_t) items,
    (size_t) size));
}