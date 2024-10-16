static void snake_upcase_char(char *c)
{
    if (*c >= 'a' && *c <= 'z')
      *c &= ~0x20;
    else if (*c == '_')
      *c = ',';
    else if (*c == '-')
      *c = '_';
}