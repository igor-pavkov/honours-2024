static int stbi__cpuid3(void)
{
   int info[4];
   __cpuid(info,1);
   return info[3];
}