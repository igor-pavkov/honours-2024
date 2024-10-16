static int readWordFromBuffer(char **s, ssize_t *len)
{
  unsigned char
    buffer[2];

  int
    i,
    c;

  for (i=0; i<2; i++)
  {
    c = *(*s)++; (*len)--;
    if (*len < 0) return -1;
    buffer[i] = (unsigned char) c;
  }
  return (((int) buffer[ 0 ]) <<  8) |
         (((int) buffer[ 1 ]));
}