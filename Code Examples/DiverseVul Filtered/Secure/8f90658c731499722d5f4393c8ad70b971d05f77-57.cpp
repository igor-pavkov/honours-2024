int RGWPutACLs_ObjStore_S3::get_params()
{
  int ret =  RGWPutACLs_ObjStore::get_params();
  if (ret >= 0) {
    const int ret_auth = do_aws4_auth_completion();
    if (ret_auth < 0) {
      return ret_auth;
    }
  } else {
    /* a request body is not required an S3 PutACLs request--n.b.,
     * s->length is non-null iff a content length was parsed (the
     * ACP or canned ACL could be in any of 3 headers, don't worry
     * about that here) */
    if ((ret == -ERR_LENGTH_REQUIRED) &&
	!!(s->length)) {
      return 0;
    }
  }
  return ret;
}