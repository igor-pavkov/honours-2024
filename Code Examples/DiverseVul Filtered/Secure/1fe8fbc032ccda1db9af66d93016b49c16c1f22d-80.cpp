static int is_item_value_atext(char ch)
{
  switch (ch) {
  case '\t':
  case ' ':
  case '\r':
  case '\n':
  case ';':
    return FALSE;
  default:
    return TRUE;
  }
}