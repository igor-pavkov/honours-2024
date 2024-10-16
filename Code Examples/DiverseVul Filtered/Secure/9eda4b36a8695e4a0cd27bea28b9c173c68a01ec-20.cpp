static void PNGType(png_bytep p,const png_byte *type)
{
  (void) memcpy(p,type,4*sizeof(png_byte));
}