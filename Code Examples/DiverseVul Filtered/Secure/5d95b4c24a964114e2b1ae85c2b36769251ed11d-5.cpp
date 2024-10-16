static void RelinquishLZMAMemory(void *context,void *memory)
{
  (void) context;
  memory=RelinquishMagickMemory(memory);
}