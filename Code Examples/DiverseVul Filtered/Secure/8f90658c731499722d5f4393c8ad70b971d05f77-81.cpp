static int verify_mfa(rgw::sal::RGWRadosStore *store, RGWUserInfo *user, const string& mfa_str, bool *verified, const DoutPrefixProvider *dpp)
{
  vector<string> params;
  get_str_vec(mfa_str, " ", params);

  if (params.size() != 2) {
    ldpp_dout(dpp, 5) << "NOTICE: invalid mfa string provided: " << mfa_str << dendl;
    return -EINVAL;
  }

  string& serial = params[0];
  string& pin = params[1];

  auto i = user->mfa_ids.find(serial);
  if (i == user->mfa_ids.end()) {
    ldpp_dout(dpp, 5) << "NOTICE: user does not have mfa device with serial=" << serial << dendl;
    return -EACCES;
  }

  int ret = store->svc()->cls->mfa.check_mfa(user->user_id, serial, pin, null_yield);
  if (ret < 0) {
    ldpp_dout(dpp, 20) << "NOTICE: failed to check MFA, serial=" << serial << dendl;
    return -EACCES;
  }

  *verified = true;

  return 0;
}