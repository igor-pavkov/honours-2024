static CURLcode tftp_translate_code(tftp_error_t error)
{
  CURLcode result = CURLE_OK;

  if(error != TFTP_ERR_NONE) {
    switch(error) {
    case TFTP_ERR_NOTFOUND:
      result = CURLE_TFTP_NOTFOUND;
      break;
    case TFTP_ERR_PERM:
      result = CURLE_TFTP_PERM;
      break;
    case TFTP_ERR_DISKFULL:
      result = CURLE_REMOTE_DISK_FULL;
      break;
    case TFTP_ERR_UNDEF:
    case TFTP_ERR_ILLEGAL:
      result = CURLE_TFTP_ILLEGAL;
      break;
    case TFTP_ERR_UNKNOWNID:
      result = CURLE_TFTP_UNKNOWNID;
      break;
    case TFTP_ERR_EXISTS:
      result = CURLE_REMOTE_FILE_EXISTS;
      break;
    case TFTP_ERR_NOSUCHUSER:
      result = CURLE_TFTP_NOSUCHUSER;
      break;
    case TFTP_ERR_TIMEOUT:
      result = CURLE_OPERATION_TIMEDOUT;
      break;
    case TFTP_ERR_NORESPONSE:
      result = CURLE_COULDNT_CONNECT;
      break;
    default:
      result = CURLE_ABORTED_BY_CALLBACK;
      break;
    }
  }
  else
    result = CURLE_OK;

  return result;
}