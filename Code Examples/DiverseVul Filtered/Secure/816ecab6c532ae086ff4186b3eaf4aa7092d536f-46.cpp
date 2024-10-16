static void LogPNGChunk(MagickBooleanType logging, const png_byte *type,
   size_t length)
{
  if (logging != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "  Writing %c%c%c%c chunk, length: %.20g",
      type[0],type[1],type[2],type[3],(double) length);
}