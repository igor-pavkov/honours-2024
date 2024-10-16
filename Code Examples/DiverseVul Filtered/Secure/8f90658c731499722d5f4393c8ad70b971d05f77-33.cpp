bool rgw::auth::s3::S3AnonymousEngine::is_applicable(
  const req_state* s
) const noexcept {
  if (s->op == OP_OPTIONS) {
    return true;
  }

  AwsVersion version;
  AwsRoute route;
  std::tie(version, route) = discover_aws_flavour(s->info);

  return route == AwsRoute::QUERY_STRING && version == AwsVersion::UNKNOWN;
}