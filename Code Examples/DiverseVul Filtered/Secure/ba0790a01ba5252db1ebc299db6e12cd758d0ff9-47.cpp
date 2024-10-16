int RGWSetBucketVersioning_ObjStore_S3::get_params()
{
  char *data = nullptr;
  int len = 0;
  int r =
    rgw_rest_read_all_input(s, &data, &len, s->cct->_conf->rgw_max_put_param_size, false);
  if (r < 0) {
    return r;
  }
  
  auto data_deleter = std::unique_ptr<char, decltype(free)*>{data, free};

  r = do_aws4_auth_completion();
  if (r < 0) {
    return r;
  }

  RGWXMLDecoder::XMLParser parser;
  if (!parser.init()) {
    ldout(s->cct, 0) << "ERROR: failed to initialize parser" << dendl;
    return -EIO;
  }

  if (!parser.parse(data, len, 1)) {
    ldout(s->cct, 10) << "NOTICE: failed to parse data: " << data << dendl;
    r = -EINVAL;
    return r;
  }

  ver_config_status status_conf;

  if (!RGWXMLDecoder::decode_xml("VersioningConfiguration", status_conf, &parser)) {
    ldout(s->cct, 10) << "NOTICE: bad versioning config input" << dendl;
    return -EINVAL;
  }

  if (!store->is_meta_master()) {
    /* only need to keep this data around if we're not meta master */
    in_data.append(data, len);
  }

  versioning_status = status_conf.status;
  if (versioning_status == VersioningStatusInvalid) {
    r = -EINVAL;
  }

  if (status_conf.mfa_status != ver_config_status::MFA_UNKNOWN) {
    mfa_set_status = true;
    switch (status_conf.mfa_status) {
      case ver_config_status::MFA_DISABLED:
        mfa_status = false;
        break;
      case ver_config_status::MFA_ENABLED:
        mfa_status = true;
        break;
      default:
        ldout(s->cct, 0) << "ERROR: RGWSetBucketVersioning_ObjStore_S3::get_params(): unexpected switch case mfa_status=" << status_conf.mfa_status << dendl;
        r = -EIO;
    }
  } else if (status_conf.retcode < 0) {
    r = status_conf.retcode;
  }
  return r;
}