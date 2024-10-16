static MngPair mng_read_pair(MngPair previous_pair,int delta_type,
  unsigned char *p)
{
  MngPair
    pair;

  /*
    Read two ssize_t's from CLON, MOVE or PAST chunk
  */
  pair.a=mng_get_long(p);
  pair.b=mng_get_long(&p[4]);
  if (delta_type != 0)
    {
      pair.a+=previous_pair.a;
      pair.b+=previous_pair.b;
    }

  return(pair);
}