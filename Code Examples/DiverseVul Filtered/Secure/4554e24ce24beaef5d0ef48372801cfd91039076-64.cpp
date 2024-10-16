void CLASS linear_table(unsigned len)
{
  int i;
  if (len > 0x10000)
    len = 0x10000;
  else if(len < 1)
    return;
  read_shorts(curve, len);
  for (i = len; i < 0x10000; i++)
    curve[i] = curve[i - 1];
  maximum = curve[len < 0x1000 ? 0xfff : len - 1];
}