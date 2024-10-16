void rgw::auth::s3::LDAPEngine::shutdown() {
  if (ldh) {
    delete ldh;
    ldh = nullptr;
  }
}