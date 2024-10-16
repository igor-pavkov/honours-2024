void rgw_get_errno_s3(rgw_http_error *e , int err_no)
{
  rgw_http_errors::const_iterator r = rgw_http_s3_errors.find(err_no);

  if (r != rgw_http_s3_errors.end()) {
    e->http_ret = r->second.first;
    e->s3_code = r->second.second;
  } else {
    e->http_ret = 500;
    e->s3_code = "UnknownError";
  }
}