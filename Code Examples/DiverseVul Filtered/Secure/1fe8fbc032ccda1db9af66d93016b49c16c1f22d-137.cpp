static int mailimf_envelope_field_parse(const char * message, size_t length,
					size_t * indx,
					struct mailimf_field ** result)
{
  size_t cur_token;
  int type;
  struct mailimf_orig_date * orig_date;
  struct mailimf_from * from;
  struct mailimf_sender * sender;
  struct mailimf_reply_to * reply_to;
  struct mailimf_to * to;
  struct mailimf_cc * cc;
  struct mailimf_bcc * bcc;
  struct mailimf_message_id * message_id;
  struct mailimf_in_reply_to * in_reply_to;
  struct mailimf_references * references;
  struct mailimf_subject * subject;
  struct mailimf_optional_field * optional_field;
  struct mailimf_field * field;
  int guessed_type;
  int r;
  int res;
  
  cur_token = * indx;

  orig_date = NULL;
  from = NULL;
  sender = NULL;
  reply_to = NULL;
  to = NULL;
  cc = NULL;
  bcc = NULL;
  message_id = NULL;
  in_reply_to = NULL;
  references = NULL;
  subject = NULL;
  optional_field = NULL;

  guessed_type = guess_header_type(message, length, cur_token);
  type = MAILIMF_FIELD_NONE;

  switch (guessed_type) {
  case MAILIMF_FIELD_ORIG_DATE:
    r = mailimf_orig_date_parse(message, length, &cur_token,
				&orig_date);
    if (r == MAILIMF_NO_ERROR)
      type = guessed_type;
    else if (r == MAILIMF_ERROR_PARSE) {
      /* do nothing */
    }
    else {
      res = r;
      goto err;
    }
    break;
  case MAILIMF_FIELD_FROM:
    r = mailimf_from_parse(message, length, &cur_token,
			   &from);
    if (r == MAILIMF_NO_ERROR)
      type = guessed_type;
    else if (r == MAILIMF_ERROR_PARSE) {
      /* do nothing */
    }
    else {
      res = r;
      goto err;
    }
    break;
  case MAILIMF_FIELD_SENDER:
    r = mailimf_sender_parse(message, length, &cur_token,
			     &sender);
    if (r == MAILIMF_NO_ERROR)
      type = guessed_type;
    else if (r == MAILIMF_ERROR_PARSE) {
      /* do nothing */
    }
    else {
      res = r;
      goto err;
    }
    break;
  case MAILIMF_FIELD_REPLY_TO:
    r = mailimf_reply_to_parse(message, length, &cur_token,
			       &reply_to);
    if (r == MAILIMF_NO_ERROR)
      type = guessed_type;
    else if (r == MAILIMF_ERROR_PARSE) {
      /* do nothing */
    }
    else {
      res = r;
      goto err;
    }
    break;
  case MAILIMF_FIELD_TO:
    r = mailimf_to_parse(message, length, &cur_token,
			 &to);
    if (r == MAILIMF_NO_ERROR)
      type = guessed_type;
    else if (r == MAILIMF_ERROR_PARSE) {
      /* do nothing */
    }
    else {
      res = r;
      goto err;
    }
    break;
  case MAILIMF_FIELD_CC:
    r = mailimf_cc_parse(message, length, &cur_token,
			 &cc);
    if (r == MAILIMF_NO_ERROR)
      type = guessed_type;
    else if (r == MAILIMF_ERROR_PARSE) {
      /* do nothing */
    }
    else {
      res = r;
      goto err;
    }
    break;
  case MAILIMF_FIELD_BCC:
    r = mailimf_bcc_parse(message, length, &cur_token,
			  &bcc);
    if (r == MAILIMF_NO_ERROR)
      type = guessed_type;
    else if (r == MAILIMF_ERROR_PARSE) {
      /* do nothing */
    }
    else {
      res = r;
      goto err;
    }
    break;
  case MAILIMF_FIELD_MESSAGE_ID:
    r = mailimf_message_id_parse(message, length, &cur_token,
				 &message_id);
    if (r == MAILIMF_NO_ERROR)
      type = guessed_type;
    else if (r == MAILIMF_ERROR_PARSE) {
      /* do nothing */
    }
    else {
      res = r;
      goto err;
    }
    break;
  case MAILIMF_FIELD_IN_REPLY_TO:
    r = mailimf_in_reply_to_parse(message, length, &cur_token,
				  &in_reply_to);
    if (r == MAILIMF_NO_ERROR)
      type = guessed_type;
    else if (r == MAILIMF_ERROR_PARSE) {
      /* do nothing */
    }
    else {
      res = r;
      goto err;
    }
    break;
  case MAILIMF_FIELD_REFERENCES:
    r = mailimf_references_parse(message, length, &cur_token,
				 &references);
    if (r == MAILIMF_NO_ERROR)
      type = guessed_type;
    else if (r == MAILIMF_ERROR_PARSE) {
      /* do nothing */
    }
    else {
      res = r;
      goto err;
    }
    break;
  case MAILIMF_FIELD_SUBJECT:
    r = mailimf_subject_parse(message, length, &cur_token,
			      &subject);
    if (r == MAILIMF_NO_ERROR)
      type = guessed_type;
    else if (r == MAILIMF_ERROR_PARSE) {
      /* do nothing */
    }
    else {
      res = r;
      goto err;
    }
    break;
  }

  if (type == MAILIMF_FIELD_NONE) {
    res = MAILIMF_ERROR_PARSE;
    goto err;
  }

  field = mailimf_field_new(type, NULL, NULL, NULL, NULL, NULL,
      NULL, NULL, NULL,
      orig_date, from, sender, reply_to, to,
      cc, bcc, message_id, in_reply_to, references,
      subject, NULL, NULL, optional_field);
  if (field == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto free_field;
  }
  
  * result = field;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;

 free_field:
  if (orig_date != NULL)
    mailimf_orig_date_free(orig_date);
  if (from != NULL)
    mailimf_from_free(from);
  if (sender != NULL)
    mailimf_sender_free(sender);
  if (reply_to != NULL)
    mailimf_reply_to_free(reply_to);
  if (to != NULL)
    mailimf_to_free(to);
  if (cc != NULL)
    mailimf_cc_free(cc);
  if (bcc != NULL)
    mailimf_bcc_free(bcc);
  if (message_id != NULL)
    mailimf_message_id_free(message_id);
  if (in_reply_to != NULL)
    mailimf_in_reply_to_free(in_reply_to);
  if (references != NULL)
    mailimf_references_free(references);
  if (subject != NULL)
    mailimf_subject_free(subject);
  if (optional_field != NULL)
    mailimf_optional_field_free(optional_field);
 err:
  return res;
}