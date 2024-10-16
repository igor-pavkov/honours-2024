static int stbi__is_16_main(stbi__context *s)
{
   #ifndef STBI_NO_PNG
   if (stbi__png_is16(s))  return 1;
   #endif

   #ifndef STBI_NO_PSD
   if (stbi__psd_is16(s))  return 1;
   #endif

   #ifndef STBI_NO_PNM
   if (stbi__pnm_is16(s))  return 1;
   #endif
   return 0;
}