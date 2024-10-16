SSLNetVConnection::protocol_contains(std::string_view prefix) const
{
  const char *retval   = nullptr;
  std::string_view tag = map_tls_protocol_to_tag(getSSLProtocol());
  if (prefix.size() <= tag.size() && strncmp(tag.data(), prefix.data(), prefix.size()) == 0) {
    retval = tag.data();
  } else {
    retval = super::protocol_contains(prefix);
  }
  return retval;
}