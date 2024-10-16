char *get_56_lenc_string(char **buffer,
                         size_t *max_bytes_available,
                         size_t *string_length)
{
  static char empty_string[1]= { '\0' };
  char *begin= *buffer;
  uchar *pos= (uchar *)begin;
  size_t required_length= 9;


  if (*max_bytes_available == 0)
    return NULL;

  /*
    If the length encoded string has the length 0
    the total size of the string is only one byte long (the size byte)
  */
  if (*begin == 0)
  {
    *string_length= 0;
    --*max_bytes_available;
    ++*buffer;
    /*
      Return a pointer to the \0 character so the return value will be
      an empty string.
    */
    return empty_string;
  }

  /* Make sure we have enough bytes available for net_field_length_ll */
  {
    DBUG_EXECUTE_IF("buffer_too_short_3",
                    *pos= 252; *max_bytes_available= 2;
    );
    DBUG_EXECUTE_IF("buffer_too_short_4",
                    *pos= 253; *max_bytes_available= 3;
    );
    DBUG_EXECUTE_IF("buffer_too_short_9",
                    *pos= 254; *max_bytes_available= 8;
    );

    if (*pos <= 251)
      required_length= 1;
    if (*pos == 252)
      required_length= 3;
    if (*pos == 253)
      required_length= 4;

    if (*max_bytes_available < required_length)
      return NULL;
  }

  *string_length= (size_t)net_field_length_ll((uchar **)buffer);

  DBUG_EXECUTE_IF("sha256_password_scramble_too_long",
                  *string_length= SIZE_T_MAX;
  );

  size_t len_len= (size_t)(*buffer - begin);

  DBUG_ASSERT((*max_bytes_available >= len_len) &&
              (len_len == required_length));
  
  if (*string_length > *max_bytes_available - len_len)
    return NULL;

  *max_bytes_available -= *string_length;
  *max_bytes_available -= len_len;
  *buffer += *string_length;
  return (char *)(begin + len_len);
}