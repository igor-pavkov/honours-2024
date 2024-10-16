static char *parse_params(
                          imp_xxh_t *imp_xxh,
                          pTHX_ MYSQL *sock,
                          char *statement,
                          STRLEN *slen_ptr,
                          imp_sth_ph_t* params,
                          int num_params,
                          bool bind_type_guessing,
                          bool bind_comment_placeholders)
{
  bool comment_end= false;
  char *salloc, *statement_ptr;
  char *statement_ptr_end, *ptr, *valbuf;
  char *cp, *end;
  int alen, i;
  int slen= *slen_ptr;
  int limit_flag= 0;
  int comment_length=0;
  STRLEN vallen;
  imp_sth_ph_t *ph;

  if (DBIc_DBISTATE(imp_xxh)->debug >= 2)
    PerlIO_printf(DBIc_LOGPIO(imp_xxh), ">parse_params statement %s\n", statement);

  if (num_params == 0)
    return NULL;

  while (isspace(*statement))
  {
    ++statement;
    --slen;
  }

  /* Calculate the number of bytes being allocated for the statement */
  alen= slen;

  for (i= 0, ph= params; i < num_params; i++, ph++)
  {
    int defined= 0;
    if (ph->value)
    {
      if (SvMAGICAL(ph->value))
        mg_get(ph->value);
      if (SvOK(ph->value))
        defined=1;
    }
    if (!defined)
      alen+= 3;  /* Erase '?', insert 'NULL' */
    else
    {
      valbuf= SvPV(ph->value, vallen);
      alen+= 2+vallen+1;
      /* this will most likely not happen since line 214 */
      /* of mysql.xs hardcodes all types to SQL_VARCHAR */
      if (!ph->type)
      {
        if (bind_type_guessing)
        {
          valbuf= SvPV(ph->value, vallen);
          ph->type= SQL_INTEGER;

          if (parse_number(valbuf, vallen, &end) != 0)
          {
              ph->type= SQL_VARCHAR;
          }
        }
        else
          ph->type= SQL_VARCHAR;
      }
    }
  }

  /* Allocate memory, why *2, well, because we have ptr and statement_ptr */
  New(908, salloc, alen*2, char);
  ptr= salloc;

  i= 0;
 /* Now create the statement string; compare count_params above */
  statement_ptr_end= (statement_ptr= statement)+ slen;

  while (statement_ptr < statement_ptr_end)
  {
    /* LIMIT should be the last part of the query, in most cases */
    if (! limit_flag)
    {
      /*
        it would be good to be able to handle any number of cases and orders
      */
      if ((*statement_ptr == 'l' || *statement_ptr == 'L') &&
          (!strncmp(statement_ptr+1, "imit ?", 6) ||
           !strncmp(statement_ptr+1, "IMIT ?", 6)))
      {
        limit_flag = 1;
      }
    }
    switch (*statement_ptr)
    {
      /* comment detection. Anything goes in a comment */
      case '-':
      {
          if (bind_comment_placeholders)
          {
              *ptr++= *statement_ptr++;
              break;
          }
          else
          {
              comment_length= 1;
              comment_end= false;
              *ptr++ = *statement_ptr++;
              if  (*statement_ptr == '-')
              {
                  /* ignore everything until newline or end of string */
                  while (*statement_ptr)
                  {
                      comment_length++;
                      *ptr++ = *statement_ptr++;
                      if (!*statement_ptr || *statement_ptr == '\n')
                      {
                          comment_end= true;
                          break;
                      }
                  }
                  /* if not end of comment, go back to where we started, no end found */
                  if (! comment_end)
                  {
                      statement_ptr -= comment_length;
                      ptr -= comment_length;
                  }
              }
              break;
          }
      }
      /* c-type comments */
      case '/':
      {
          if (bind_comment_placeholders)
          {
              *ptr++= *statement_ptr++;
              break;
          }
          else
          {
              comment_length= 1;
              comment_end= false;
              *ptr++ = *statement_ptr++;
              if  (*statement_ptr == '*')
              {
                  /* use up characters everything until newline */
                  while (*statement_ptr)
                  {
                      *ptr++ = *statement_ptr++;
                      comment_length++;
                      if (!strncmp(statement_ptr, "*/", 2))
                      {
                          comment_length += 2;
                          comment_end= true;
                          break;
                      }
                  }
                  /* Go back to where started if comment end not found */
                  if (! comment_end)
                  {
                      statement_ptr -= comment_length;
                      ptr -= comment_length;
                  }
              }
              break;
          }
      }
      case '`':
      case '\'':
      case '"':
      /* Skip string */
      {
        char endToken = *statement_ptr++;
        *ptr++ = endToken;
        while (statement_ptr != statement_ptr_end &&
               *statement_ptr != endToken)
        {
          if (*statement_ptr == '\\')
          {
            *ptr++ = *statement_ptr++;
            if (statement_ptr == statement_ptr_end)
	      break;
	  }
          *ptr++= *statement_ptr++;
	}
	if (statement_ptr != statement_ptr_end)
          *ptr++= *statement_ptr++;
      }
      break;

      case '?':
        /* Insert parameter */
        statement_ptr++;
        if (i >= num_params)
        {
          break;
        }

        ph = params+ (i++);
        if (!ph->value  ||  !SvOK(ph->value))
        {
          *ptr++ = 'N';
          *ptr++ = 'U';
          *ptr++ = 'L';
          *ptr++ = 'L';
        }
        else
        {
          int is_num = FALSE;

          valbuf= SvPV(ph->value, vallen);
          if (valbuf)
          {
            switch (ph->type)
            {
              case SQL_NUMERIC:
              case SQL_DECIMAL:
              case SQL_INTEGER:
              case SQL_SMALLINT:
              case SQL_FLOAT:
              case SQL_REAL:
              case SQL_DOUBLE:
              case SQL_BIGINT:
              case SQL_TINYINT:
                is_num = TRUE;
                break;
            }

            /* (note this sets *end, which we use if is_num) */
            if ( parse_number(valbuf, vallen, &end) != 0 && is_num)
            {
              if (bind_type_guessing) {
                /* .. not a number, so apparently we guessed wrong */
                is_num = 0;
                ph->type = SQL_VARCHAR;
              }
            }


            /* we're at the end of the query, so any placeholders if */
            /* after a LIMIT clause will be numbers and should not be quoted */
            if (limit_flag == 1)
              is_num = TRUE;

            if (!is_num)
            {
              *ptr++ = '\'';
              ptr += mysql_real_escape_string(sock, ptr, valbuf, vallen);
              *ptr++ = '\'';
            }
            else
            {
              for (cp= valbuf; cp < end; cp++)
                  *ptr++= *cp;
            }
          }
        }
        break;

	/* in case this is a nested LIMIT */
      case ')':
        limit_flag = 0;
	*ptr++ = *statement_ptr++;
        break;

      default:
        *ptr++ = *statement_ptr++;
        break;

    }
  }

  *slen_ptr = ptr - salloc;
  *ptr++ = '\0';

  return(salloc);
}