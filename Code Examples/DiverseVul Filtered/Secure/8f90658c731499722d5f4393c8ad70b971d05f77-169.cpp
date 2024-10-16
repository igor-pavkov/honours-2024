AWSBrowserUploadAbstractor::get_auth_data(const req_state* const s) const
{
  if (s->auth.s3_postobj_creds.x_amz_algorithm == AWS4_HMAC_SHA256_STR) {
    ldpp_dout(s, 0) << "Signature verification algorithm AWS v4"
                     << " (AWS4-HMAC-SHA256)" << dendl;
    return get_auth_data_v4(s);
  } else {
    ldpp_dout(s, 0) << "Signature verification algorithm AWS v2" << dendl;
    return get_auth_data_v2(s);
  }
}