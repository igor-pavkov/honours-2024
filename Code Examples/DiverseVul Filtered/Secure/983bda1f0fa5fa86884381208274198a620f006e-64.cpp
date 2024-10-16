void CLASS read_shorts (ushort *pixel, unsigned count)
{
  if (fread (pixel, 2, count, ifp) < count) derror();
  if ((order == 0x4949) == (ntohs(0x1234) == 0x1234))
    swab ((const char *)pixel, (char *)pixel, count*2); /*mingw support UF*/
}