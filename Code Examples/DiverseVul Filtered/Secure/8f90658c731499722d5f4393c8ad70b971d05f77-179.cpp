int RGWHandler_REST_S3::authorize(const DoutPrefixProvider *dpp)
{
  if (s->info.args.exists("Action") && s->info.args.get("Action") == "AssumeRoleWithWebIdentity") {
    return RGW_Auth_STS::authorize(dpp, store, auth_registry, s);
  }
  return RGW_Auth_S3::authorize(dpp, store, auth_registry, s);
}