AWSGeneralAbstractor::get_v4_canonical_headers(
  const req_info& info,
  const boost::string_view& signedheaders,
  const bool using_qs) const
{
  return rgw::auth::s3::get_v4_canonical_headers(info, signedheaders,
                                                 using_qs, false);
}