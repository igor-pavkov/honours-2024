int RGWConfigBucketMetaSearch_ObjStore_S3::get_params()
{
  auto iter = s->info.x_meta_map.find("x-amz-meta-search");
  if (iter == s->info.x_meta_map.end()) {
    s->err.message = "X-Rgw-Meta-Search header not provided";
    ldpp_dout(this, 5) << s->err.message << dendl;
    return -EINVAL;
  }

  list<string> expressions;
  get_str_list(iter->second, ",", expressions);

  for (auto& expression : expressions) {
    vector<string> args;
    get_str_vec(expression, ";", args);

    if (args.empty()) {
      s->err.message = "invalid empty expression";
      ldpp_dout(this, 5) << s->err.message << dendl;
      return -EINVAL;
    }
    if (args.size() > 2) {
      s->err.message = string("invalid expression: ") + expression;
      ldpp_dout(this, 5) << s->err.message << dendl;
      return -EINVAL;
    }

    string key = boost::algorithm::to_lower_copy(rgw_trim_whitespace(args[0]));
    string val;
    if (args.size() > 1) {
      val = boost::algorithm::to_lower_copy(rgw_trim_whitespace(args[1]));
    }

    if (!boost::algorithm::starts_with(key, RGW_AMZ_META_PREFIX)) {
      s->err.message = string("invalid expression, key must start with '" RGW_AMZ_META_PREFIX "' : ") + expression;
      ldpp_dout(this, 5) << s->err.message << dendl;
      return -EINVAL;
    }

    key = key.substr(sizeof(RGW_AMZ_META_PREFIX) - 1);

    ESEntityTypeMap::EntityType entity_type;

    if (val.empty() || val == "str" || val == "string") {
      entity_type = ESEntityTypeMap::ES_ENTITY_STR;
    } else if (val == "int" || val == "integer") {
      entity_type = ESEntityTypeMap::ES_ENTITY_INT;
    } else if (val == "date" || val == "datetime") {
      entity_type = ESEntityTypeMap::ES_ENTITY_DATE;
    } else {
      s->err.message = string("invalid entity type: ") + val;
      ldpp_dout(this, 5) << s->err.message << dendl;
      return -EINVAL;
    }

    mdsearch_config[key] = entity_type;
  }

  return 0;
}