static size_t GetIPTCStream(unsigned char **info,size_t length)
{
  int
    c;

  register ssize_t
    i;

  register unsigned char
    *p;

  size_t
    extent,
    info_length;

  unsigned int
    marker;

  size_t
    tag_length;

  p=(*info);
  extent=length;
  if ((*p == 0x1c) && (*(p+1) == 0x02))
    return(length);
  /*
    Extract IPTC from 8BIM resource block.
  */
  while (extent >= 12)
  {
    if (strncmp((const char *) p,"8BIM",4))
      break;
    p+=4;
    extent-=4;
    marker=(unsigned int) (*p) << 8 | *(p+1);
    p+=2;
    extent-=2;
    c=*p++;
    extent--;
    c|=0x01;
    if ((size_t) c >= extent)
      break;
    p+=c;
    extent-=c;
    if (extent < 4)
      break;
    tag_length=(((size_t) *p) << 24) | (((size_t) *(p+1)) << 16) |
      (((size_t) *(p+2)) << 8) | ((size_t) *(p+3));
    p+=4;
    extent-=4;
    if (tag_length > extent)
      break;
    if (marker == IPTC_ID)
      {
        *info=p;
        return(tag_length);
      }
    if ((tag_length & 0x01) != 0)
      tag_length++;
    p+=tag_length;
    extent-=tag_length;
  }
  /*
    Find the beginning of the IPTC info.
  */
  p=(*info);
  tag_length=0;
iptc_find:
  info_length=0;
  marker=MagickFalse;
  while (length != 0)
  {
    c=(*p++);
    length--;
    if (length == 0)
      break;
    if (c == 0x1c)
      {
        p--;
        *info=p; /* let the caller know were it is */
        break;
      }
  }
  /*
    Determine the length of the IPTC info.
  */
  while (length != 0)
  {
    c=(*p++);
    length--;
    if (length == 0)
      break;
    if (c == 0x1c)
      marker=MagickTrue;
    else
      if (marker)
        break;
      else
        continue;
    info_length++;
    /*
      Found the 0x1c tag; skip the dataset and record number tags.
    */
    c=(*p++); /* should be 2 */
    length--;
    if (length == 0)
      break;
    if ((info_length == 1) && (c != 2))
      goto iptc_find;
    info_length++;
    c=(*p++); /* should be 0 */
    length--;
    if (length == 0)
      break;
    if ((info_length == 2) && (c != 0))
      goto iptc_find;
    info_length++;
    /*
      Decode the length of the block that follows - ssize_t or short format.
    */
    c=(*p++);
    length--;
    if (length == 0)
      break;
    info_length++;
    if ((c & 0x80) != 0)
      {
        /*
          Long format.
        */
        tag_length=0;
        for (i=0; i < 4; i++)
        {
          tag_length<<=8;
          tag_length|=(*p++);
          length--;
          if (length == 0)
            break;
          info_length++;
        }
      }
    else
      {
        /*
          Short format.
        */
        tag_length=((long) c) << 8;
        c=(*p++);
        length--;
        if (length == 0)
          break;
        info_length++;
        tag_length|=(long) c;
      }
    if (tag_length > (length+1))
      break;
    p+=tag_length;
    length-=tag_length;
    if (length == 0)
      break;
    info_length+=tag_length;
  }
  return(info_length);
}