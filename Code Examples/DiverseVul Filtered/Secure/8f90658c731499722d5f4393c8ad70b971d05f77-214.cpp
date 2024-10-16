rgw::auth::s3::LDAPEngine::get_creds_info(const rgw::RGWToken& token) const noexcept
{
  /* The short form of "using" can't be used here -- we're aliasing a class'
   * member. */
  using acct_privilege_t = \
    rgw::auth::RemoteApplier::AuthInfo::acct_privilege_t;

  return rgw::auth::RemoteApplier::AuthInfo {
    rgw_user(token.id),
    token.id,
    RGW_PERM_FULL_CONTROL,
    acct_privilege_t::IS_PLAIN_ACCT,
    TYPE_LDAP
  };
}