static void PNGType(png_bytep p,const png_byte *type)
{
  (void) CopyMagickMemory(p,type,4*sizeof(png_byte));
}