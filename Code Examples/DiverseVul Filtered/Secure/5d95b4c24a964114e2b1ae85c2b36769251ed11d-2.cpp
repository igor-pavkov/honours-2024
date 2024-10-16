static void RelinquishBZIPMemory(void *context,void *memory)
{
  (void) context;
  memory=RelinquishMagickMemory(memory);
}