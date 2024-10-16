static int mailimf_item_value_parse(const char * message, size_t length,
				    size_t * indx,
				    struct mailimf_item_value ** result)
{
  size_t cur_token;
  clist * angle_addr_list;
  char * addr_spec;
  char * atom;
  char * domain;
  char * msg_id;
  int type;
  struct mailimf_item_value * item_value;
  int r;
  int res;

  cur_token = * indx;
  
  angle_addr_list = NULL;
  addr_spec = NULL;
  atom = NULL;
  domain = NULL;
  msg_id = NULL;

  r = mailimf_struct_multiple_parse(message, length, &cur_token,
				    &angle_addr_list,
				    (mailimf_struct_parser *)
				    mailimf_angle_addr_parse,
				    (mailimf_struct_destructor *)
				    mailimf_angle_addr_free);
  if (r == MAILIMF_NO_ERROR)
    type = MAILIMF_ITEM_VALUE_ANGLE_ADDR_LIST;

  if (r == MAILIMF_ERROR_PARSE) {
    r = mailimf_addr_spec_parse(message, length, &cur_token,
				&addr_spec);
    if (r == MAILIMF_NO_ERROR)
      type = MAILIMF_ITEM_VALUE_ADDR_SPEC;
  }

  if (r == MAILIMF_ERROR_PARSE) {
    r = mailimf_msg_id_parse(message, length, &cur_token,
			     &msg_id);
    if (r == MAILIMF_NO_ERROR)
      type = MAILIMF_ITEM_VALUE_MSG_ID;
  }

  /*
  else if (mailimf_domain_parse(message, length, &cur_token,
				&domain))
    type = MAILIMF_ITEM_VALUE_DOMAIN;
  */
  /*
  else if (mailimf_atom_parse(message, length, &cur_token,
			      &atom))
    type = MAILIMF_ITEM_VALUE_ATOM;
  */

  if (r == MAILIMF_ERROR_PARSE) {
    r = mailimf_item_value_atom_parse(message, length, &cur_token,
				      &atom);
    if (r == MAILIMF_NO_ERROR)
      type = MAILIMF_ITEM_VALUE_ATOM;
  }

  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  item_value = mailimf_item_value_new(type, angle_addr_list, addr_spec,
				      atom, domain, msg_id);
  if (item_value == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto free;
  }

  * result = item_value;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;

 free:
  if (angle_addr_list != NULL) {
    clist_foreach(angle_addr_list, (clist_func) mailimf_angle_addr_free, NULL);
    clist_free(angle_addr_list);
  }
  if (addr_spec != NULL)
    mailimf_addr_spec_free(addr_spec);
  if (atom != NULL)
    mailimf_atom_free(atom);
  if (domain != NULL)
    mailimf_domain_free(domain);
  if (msg_id != NULL)
    mailimf_msg_id_free(msg_id);
 err:
  return res;
}