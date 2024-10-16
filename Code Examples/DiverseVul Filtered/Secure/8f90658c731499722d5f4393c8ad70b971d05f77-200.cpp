int RGWPutObjRetention_ObjStore_S3::get_params()
{
  const char *bypass_gov_header = s->info.env->get("HTTP_X_AMZ_BYPASS_GOVERNANCE_RETENTION");
  if (bypass_gov_header) {
    std::string bypass_gov_decoded = url_decode(bypass_gov_header);
    bypass_governance_mode = boost::algorithm::iequals(bypass_gov_decoded, "true");
  }

  const auto max_size = s->cct->_conf->rgw_max_put_param_size;
  std::tie(op_ret, data) = rgw_rest_read_all_input(s, max_size, false);
  return op_ret;
}