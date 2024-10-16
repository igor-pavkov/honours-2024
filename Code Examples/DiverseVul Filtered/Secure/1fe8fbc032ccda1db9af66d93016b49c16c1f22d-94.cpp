static int guess_resent_header_type(char * message,
				    size_t length, size_t indx)
{
  int r;

  r = mailimf_token_case_insensitive_parse(message,
					   length, &indx, "Resent-");
  if (r != MAILIMF_NO_ERROR)
    return MAILIMF_RESENT_FIELD_NONE;
  
  if (indx >= length)
    return MAILIMF_RESENT_FIELD_NONE;

  switch(toupper(message[indx])) {
  case 'D':
    return MAILIMF_RESENT_FIELD_DATE;
  case 'F':
    return MAILIMF_RESENT_FIELD_FROM;
  case 'S':
    return MAILIMF_RESENT_FIELD_SENDER;
  case 'T':
    return MAILIMF_RESENT_FIELD_TO;
  case 'C':
    return MAILIMF_RESENT_FIELD_CC;
  case 'B':
    return MAILIMF_RESENT_FIELD_BCC;
  case 'M':
    return MAILIMF_RESENT_FIELD_MSG_ID;
  default:
    return MAILIMF_RESENT_FIELD_NONE;
  }
}