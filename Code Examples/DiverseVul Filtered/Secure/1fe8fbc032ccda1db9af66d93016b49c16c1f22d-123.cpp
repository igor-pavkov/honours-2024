static int mailimf_received_parse(const char * message, size_t length,
				  size_t * indx,
				  struct mailimf_received ** result)
{
  size_t cur_token;
  struct mailimf_received * received;
  struct mailimf_name_val_list * name_val_list;
  struct mailimf_date_time * date_time;
  int r;
  int res;

  cur_token = * indx;

  r = mailimf_token_case_insensitive_parse(message, length,
					   &cur_token, "Received");
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }
  
  r = mailimf_colon_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_name_val_list_parse(message, length,
				  &cur_token, &name_val_list);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_semi_colon_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto free_name_val_list;
  }

  r = mailimf_date_time_parse(message, length, &cur_token, &date_time);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto free_name_val_list;
  }

  r = mailimf_unstrict_crlf_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto free_date_time;
  }

  received = mailimf_received_new(name_val_list, date_time);
  if (received == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto free_date_time;
  }

  * indx = cur_token;
  * result = received;

  return MAILIMF_NO_ERROR;

 free_date_time:
  mailimf_date_time_free(date_time);
 free_name_val_list:
  mailimf_name_val_list_free(name_val_list);
 err:
  return res;
}