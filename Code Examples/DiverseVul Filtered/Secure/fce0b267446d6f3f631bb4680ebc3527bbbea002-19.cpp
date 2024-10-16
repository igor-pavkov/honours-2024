static int verify_mfa(RGWRados *store, RGWUserInfo *user, const string& mfa_str, bool *verified)
{
  vector<string> params;
  get_str_vec(mfa_str, " ", params);

  if (params.size() != 2) {
    ldout(store->ctx(), 5) << "NOTICE: invalid mfa string provided: " << mfa_str << dendl;
    return -EINVAL;
  }

  string& serial = params[0];
  string& pin = params[1];

  auto i = user->mfa_ids.find(serial);
  if (i == user->mfa_ids.end()) {
    ldout(store->ctx(), 5) << "NOTICE: user does not have mfa device with serial=" << serial << dendl;
    return -EACCES;
  }

  int ret = store->check_mfa(user->user_id, serial, pin);
  if (ret < 0) {
    ldout(store->ctx(), 20) << "NOTICE: failed to check MFA, serial=" << serial << dendl;
    return -EACCES;
  }

  *verified = true;

  return 0;
}