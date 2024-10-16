static int stbi__err(const char *str)
{
   stbi__g_failure_reason = str;
   return 0;
}