static int mem_is_in_memp(int mem, int num, MemNumType* memp)
{
  int i;

  for (i = 0; i < num; i++) {
    if (mem == (int )memp[i]) return 1;
  }
  return 0;
}