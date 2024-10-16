static void StoreToken(TokenInfo *token_info,char *string,
  size_t max_token_length,int c)
{
  register ssize_t
    i;

  if ((token_info->offset < 0) ||
      ((size_t) token_info->offset >= (max_token_length-1)))
    return;
  i=token_info->offset++;
  string[i]=(char) c;
  if (token_info->state == IN_QUOTE)
    return;
  switch (token_info->flag & 0x03)
  {
    case 1:
    {
      string[i]=(char) toupper(c);
      break;
    }
    case 2:
    {
      string[i]=(char) tolower(c);
      break;
    }
    default:
      break;
  }
}