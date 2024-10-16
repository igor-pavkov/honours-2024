static int mailimf_trace_parse(const char * message, size_t length,
			       size_t * indx,
			       struct mailimf_trace ** result)
{
  size_t cur_token;
  struct mailimf_return * return_path;
  clist * received_list;
  struct mailimf_trace * trace;
  int r;
  int res;

  cur_token = * indx;
  return_path = NULL;
  received_list = NULL;

  r = mailimf_return_parse(message, length, &cur_token, &return_path);
  if ((r != MAILIMF_NO_ERROR) && (r != MAILIMF_ERROR_PARSE)) {
    res = r;
    goto err;
  }

  r = mailimf_struct_multiple_parse(message, length, &cur_token,
				    &received_list,
				    (mailimf_struct_parser *)
				    mailimf_received_parse,
				    (mailimf_struct_destructor *)
				    mailimf_received_free);
  if ((r != MAILIMF_NO_ERROR) && (r != MAILIMF_ERROR_PARSE)) {
    res = r;
    goto err;
  }

  if ((received_list == NULL) && (return_path == NULL)) {
    res = MAILIMF_ERROR_PARSE;
    goto free_return;
  }

  trace = mailimf_trace_new(return_path, received_list);
  if (trace == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto free_list;
  }

  * result = trace;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;

 free_list:
  clist_foreach(received_list, (clist_func) mailimf_received_free, NULL);
  clist_free(received_list);
 free_return:
  if (return_path != NULL)
    mailimf_return_free(return_path);
 err:
  return res;
}