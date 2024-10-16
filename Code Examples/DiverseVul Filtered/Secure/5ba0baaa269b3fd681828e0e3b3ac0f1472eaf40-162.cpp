static int stbi__pnm_is16(stbi__context *s)
{
   if (stbi__pnm_info(s, NULL, NULL, NULL) == 16)
	   return 1;
   return 0;
}