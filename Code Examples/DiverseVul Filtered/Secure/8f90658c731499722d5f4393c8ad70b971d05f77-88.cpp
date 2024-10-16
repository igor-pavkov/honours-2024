bool rgw::auth::s3::LDAPEngine::valid() {
  std::lock_guard<std::mutex> lck(mtx);
  return (!!ldh);
}