void rgw::auth::s3::LDAPEngine::init(CephContext* const cct)
{
  if (! cct->_conf->rgw_s3_auth_use_ldap ||
      cct->_conf->rgw_ldap_uri.empty()) {
    return;
  }

  if (! ldh) {
    std::lock_guard<std::mutex> lck(mtx);
    if (! ldh) {
      const string& ldap_uri = cct->_conf->rgw_ldap_uri;
      const string& ldap_binddn = cct->_conf->rgw_ldap_binddn;
      const string& ldap_searchdn = cct->_conf->rgw_ldap_searchdn;
      const string& ldap_searchfilter = cct->_conf->rgw_ldap_searchfilter;
      const string& ldap_dnattr = cct->_conf->rgw_ldap_dnattr;
      std::string ldap_bindpw = parse_rgw_ldap_bindpw(cct);

      ldh = new rgw::LDAPHelper(ldap_uri, ldap_binddn, ldap_bindpw,
                                ldap_searchdn, ldap_searchfilter, ldap_dnattr);

      ldh->init();
      ldh->bind();
    }
  }
}