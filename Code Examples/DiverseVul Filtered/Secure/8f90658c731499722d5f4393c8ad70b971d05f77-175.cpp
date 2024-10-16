rgw::auth::s3::STSEngine::get_creds_info(const STS::SessionToken& token) const noexcept
{
  using acct_privilege_t = \
    rgw::auth::RemoteApplier::AuthInfo::acct_privilege_t;

  return rgw::auth::RemoteApplier::AuthInfo {
    token.user,
    token.acct_name,
    token.perm_mask,
    (token.is_admin) ? acct_privilege_t::IS_ADMIN_ACCT: acct_privilege_t::IS_PLAIN_ACCT,
    token.acct_type
  };
}