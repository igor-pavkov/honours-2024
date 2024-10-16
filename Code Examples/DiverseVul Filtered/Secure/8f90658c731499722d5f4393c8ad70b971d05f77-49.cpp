AWSBrowserUploadAbstractor::get_auth_data_v4(const req_state* const s) const
{
  const boost::string_view credential = s->auth.s3_postobj_creds.x_amz_credential;

  /* grab access key id */
  const size_t pos = credential.find("/");
  const boost::string_view access_key_id = credential.substr(0, pos);
  dout(10) << "access key id = " << access_key_id << dendl;

  /* grab credential scope */
  const boost::string_view credential_scope = credential.substr(pos + 1);
  dout(10) << "credential scope = " << credential_scope << dendl;

  const auto sig_factory = std::bind(rgw::auth::s3::get_v4_signature,
                                     credential_scope,
                                     std::placeholders::_1,
                                     std::placeholders::_2,
                                     std::placeholders::_3);

  return {
    access_key_id,
    s->auth.s3_postobj_creds.signature,
    s->auth.s3_postobj_creds.x_amz_security_token,
    s->auth.s3_postobj_creds.encoded_policy.to_str(),
    sig_factory,
    null_completer_factory
  };
}