static voidpf AcquireZIPMemory(voidpf context,unsigned int items,
  unsigned int size)
{
  (void) context;
  return((voidpf) AcquireQuantumMemory(items,size));
}