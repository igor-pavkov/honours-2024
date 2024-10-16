static inline int is_ctext(char ch)
{
  unsigned char uch = (unsigned char) ch;

  if (is_no_ws_ctl(ch))
    return TRUE;

  if (uch < 33)
    return FALSE;

  if ((uch == 40) || (uch == 41))
    return FALSE;
  
  if (uch == 92)
    return FALSE;

  if (uch == 127)
    return FALSE;

  return TRUE;
}