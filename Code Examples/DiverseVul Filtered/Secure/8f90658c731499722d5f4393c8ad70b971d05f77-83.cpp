AWSEngine::authenticate(const DoutPrefixProvider* dpp, const req_state* const s) const
{
  /* Small reminder: an ver_abstractor is allowed to throw! */
  const auto auth_data = ver_abstractor.get_auth_data(s);

  if (auth_data.access_key_id.empty() || auth_data.client_signature.empty()) {
    return result_t::deny(-EINVAL);
  } else {
    return authenticate(dpp,
                        auth_data.access_key_id,
		        auth_data.client_signature,
            auth_data.session_token,
			auth_data.string_to_sign,
                        auth_data.signature_factory,
			auth_data.completer_factory,
			s);
  }
}