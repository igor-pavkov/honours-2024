rgw::auth::s3::LocalEngine::authenticate(
  const boost::string_view& _access_key_id,
  const boost::string_view& signature,
  const string_to_sign_t& string_to_sign,
  const signature_factory_t& signature_factory,
  const completer_factory_t& completer_factory,
  const req_state* const s) const
{
  /* get the user info */
  RGWUserInfo user_info;
  /* TODO(rzarzynski): we need to have string-view taking variant. */
  const std::string access_key_id = _access_key_id.to_string();
  if (rgw_get_user_info_by_access_key(store, access_key_id, user_info) < 0) {
      ldout(cct, 5) << "error reading user info, uid=" << access_key_id
              << " can't authenticate" << dendl;
      return result_t::deny(-ERR_INVALID_ACCESS_KEY);
  }
  //TODO: Uncomment, when we have a migration plan in place.
  /*else {
    if (s->user->type != TYPE_RGW) {
      ldout(cct, 10) << "ERROR: User id of type: " << s->user->type
                     << " is present" << dendl;
      throw -EPERM;
    }
  }*/

  const auto iter = user_info.access_keys.find(access_key_id);
  if (iter == std::end(user_info.access_keys)) {
    ldout(cct, 0) << "ERROR: access key not encoded in user info" << dendl;
    return result_t::deny(-EPERM);
  }
  const RGWAccessKey& k = iter->second;

  const VersionAbstractor::server_signature_t server_signature = \
    signature_factory(cct, k.key, string_to_sign);
  auto compare = signature.compare(server_signature);

  ldout(cct, 15) << "string_to_sign="
                 << rgw::crypt_sanitize::log_content{string_to_sign}
                 << dendl;
  ldout(cct, 15) << "server signature=" << server_signature << dendl;
  ldout(cct, 15) << "client signature=" << signature << dendl;
  ldout(cct, 15) << "compare=" << compare << dendl;

  if (compare != 0) {
    return result_t::deny(-ERR_SIGNATURE_NO_MATCH);
  }

  auto apl = apl_factory->create_apl_local(cct, s, user_info, k.subuser);
  return result_t::grant(std::move(apl), completer_factory(k.key));
}