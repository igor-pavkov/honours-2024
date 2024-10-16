int RGWCreateBucket_ObjStore_S3::get_params()
{
  RGWAccessControlPolicy_S3 s3policy(s->cct);
  bool relaxed_names = s->cct->_conf->rgw_relaxed_s3_bucket_names;
 
  int r = valid_s3_bucket_name(s->bucket_name, relaxed_names);
  if (r)
    return r;

  r = create_s3_policy(s, store, s3policy, s->owner);
  if (r < 0)
    return r;

  policy = s3policy;

  const auto max_size = s->cct->_conf->rgw_max_put_param_size;

  int op_ret = 0;
  bufferlist data;
  std::tie(op_ret, data) = rgw_rest_read_all_input(s, max_size, false);

  if ((op_ret < 0) && (op_ret != -ERR_LENGTH_REQUIRED))
    return op_ret;

  const int auth_ret = do_aws4_auth_completion();
  if (auth_ret < 0) {
    return auth_ret;
  }
  
  in_data.append(data);

  if (data.length()) {
    RGWCreateBucketParser parser;

    if (!parser.init()) {
      ldpp_dout(this, 0) << "ERROR: failed to initialize parser" << dendl;
      return -EIO;
    }

    char* buf = data.c_str();
    bool success = parser.parse(buf, data.length(), 1);
    ldpp_dout(this, 20) << "create bucket input data=" << buf << dendl;

    if (!success) {
      ldpp_dout(this, 0) << "failed to parse input: " << buf << dendl;
      return -EINVAL;
    }

    if (!parser.get_location_constraint(location_constraint)) {
      ldpp_dout(this, 0) << "provided input did not specify location constraint correctly" << dendl;
      return -EINVAL;
    }

    ldpp_dout(this, 10) << "create bucket location constraint: "
		      << location_constraint << dendl;
  }

  size_t pos = location_constraint.find(':');
  if (pos != string::npos) {
    placement_rule.init(location_constraint.substr(pos + 1), s->info.storage_class);
    location_constraint = location_constraint.substr(0, pos);
  } else {
    placement_rule.storage_class = s->info.storage_class;
  }
  auto iter = s->info.x_meta_map.find("x-amz-bucket-object-lock-enabled");
  if (iter != s->info.x_meta_map.end()) {
    if (!boost::algorithm::iequals(iter->second, "true") && !boost::algorithm::iequals(iter->second, "false")) {
      return -EINVAL;
    }
    obj_lock_enabled = boost::algorithm::iequals(iter->second, "true");
  }
  return 0;
}