static int mailimf_path_parse(const char * message, size_t length,
			      size_t * indx, struct mailimf_path ** result)
{
  size_t cur_token;
  char * addr_spec;
  struct mailimf_path * path;
  int res;
  int r;

  cur_token = * indx;
  addr_spec = NULL;

  r = mailimf_cfws_parse(message, length, &cur_token);
  if ((r != MAILIMF_NO_ERROR) && (r != MAILIMF_ERROR_PARSE)) {
    res = r;
    goto err;
  }

  r = mailimf_lower_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_addr_spec_parse(message, length, &cur_token, &addr_spec);
  switch (r) {
  case MAILIMF_NO_ERROR:
    break;
  case MAILIMF_ERROR_PARSE:
    r = mailimf_cfws_parse(message, length, &cur_token);
    if ((r != MAILIMF_NO_ERROR) && (r != MAILIMF_ERROR_PARSE)) {
      res = r;
      goto err;
    }
    break;
  default:
    return r;
  }
  
  r = mailimf_greater_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  path = mailimf_path_new(addr_spec);
  if (path == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto free_addr_spec;
  }

  * indx = cur_token;
  * result = path;

  return MAILIMF_NO_ERROR;

 free_addr_spec:
  if (addr_spec == NULL)
    mailimf_addr_spec_free(addr_spec);
 err:
  return res;
}