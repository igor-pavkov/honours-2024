AWSBrowserUploadAbstractor::get_auth_data_v2(const req_state* const s) const
{
  return {
    s->auth.s3_postobj_creds.access_key,
    s->auth.s3_postobj_creds.signature,
    s->auth.s3_postobj_creds.x_amz_security_token,
    s->auth.s3_postobj_creds.encoded_policy.to_str(),
    rgw::auth::s3::get_v2_signature,
    null_completer_factory
  };
}