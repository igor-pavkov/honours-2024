MagickExport int Tokenizer(TokenInfo *token_info,const unsigned flag,
  char *token,const size_t max_token_length,const char *line,const char *white,
  const char *break_set,const char *quote,const char escape,char *breaker,
  int *next,char *quoted)
{
  int
    c;

  register ssize_t
    i;

  *breaker='\0';
  *quoted='\0';
  if (line[*next] == '\0')
    return(1);
  token_info->state=IN_WHITE;
  token_info->quote=(char) MagickFalse;
  token_info->flag=flag;
  for (token_info->offset=0; (int) line[*next] != 0; (*next)++)
  {
    c=(int) line[*next];
    i=sindex(c,break_set);
    if (i >= 0)
      {
        switch (token_info->state)
        {
          case IN_WHITE:
          case IN_TOKEN:
          case IN_OZONE:
          {
            (*next)++;
            *breaker=break_set[i];
            token[token_info->offset]='\0';
            return(0);
          }
          case IN_QUOTE:
          {
            StoreToken(token_info,token,max_token_length,c);
            break;
          }
        }
        continue;
      }
    i=sindex(c,quote);
    if (i >= 0)
      {
        switch (token_info->state)
        {
          case IN_WHITE:
          {
            token_info->state=IN_QUOTE;
            token_info->quote=quote[i];
            *quoted=(char) MagickTrue;
            break;
          }
          case IN_QUOTE:
          {
            if (quote[i] != token_info->quote)
              StoreToken(token_info,token,max_token_length,c);
            else
              {
                token_info->state=IN_OZONE;
                token_info->quote='\0';
              }
            break;
          }
          case IN_TOKEN:
          case IN_OZONE:
          {
            *breaker=(char) c;
            token[token_info->offset]='\0';
            return(0);
          }
        }
        continue;
      }
    i=sindex(c,white);
    if (i >= 0)
      {
        switch (token_info->state)
        {
          case IN_WHITE:
          case IN_OZONE:
            break;
          case IN_TOKEN:
          {
            token_info->state=IN_OZONE;
            break;
          }
          case IN_QUOTE:
          {
            StoreToken(token_info,token,max_token_length,c);
            break;
          }
        }
        continue;
      }
    if (c == (int) escape)
      {
        if (line[(*next)+1] == '\0')
          {
            *breaker='\0';
            StoreToken(token_info,token,max_token_length,c);
            (*next)++;
            token[token_info->offset]='\0';
            return(0);
          }
        switch (token_info->state)
        {
          case IN_WHITE:
          {
            (*next)--;
            token_info->state=IN_TOKEN;
            break;
          }
          case IN_TOKEN:
          case IN_QUOTE:
          {
            (*next)++;
            c=(int) line[*next];
            StoreToken(token_info,token,max_token_length,c);
            break;
          }
          case IN_OZONE:
          {
            token[token_info->offset]='\0';
            return(0);
          }
        }
        continue;
      }
    switch (token_info->state)
    {
      case IN_WHITE:
      {
        token_info->state=IN_TOKEN;
        StoreToken(token_info,token,max_token_length,c);
        break;
      }
      case IN_TOKEN:
      case IN_QUOTE:
      {
        StoreToken(token_info,token,max_token_length,c);
        break;
      }
      case IN_OZONE:
      {
        token[token_info->offset]='\0';
        return(0);
      }
    }
  }
  token[token_info->offset]='\0';
  return(0);
}