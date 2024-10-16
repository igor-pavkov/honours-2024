AWSGeneralAbstractor::get_auth_data(const req_state* const s) const
{
  AwsVersion version;
  AwsRoute route;
  std::tie(version, route) = discover_aws_flavour(s->info);

  if (version == AwsVersion::V2) {
    return get_auth_data_v2(s);
  } else if (version == AwsVersion::V4) {
    return get_auth_data_v4(s, route == AwsRoute::QUERY_STRING);
  } else {
    /* FIXME(rzarzynski): handle anon user. */
    throw -EINVAL;
  }
}