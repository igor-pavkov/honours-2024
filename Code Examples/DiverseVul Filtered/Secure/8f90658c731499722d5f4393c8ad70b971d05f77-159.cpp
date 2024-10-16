int RGWListBucket_ObjStore_S3::get_common_params()
{
  list_versions = s->info.args.exists("versions");
  prefix = s->info.args.get("prefix");

  // non-standard
  s->info.args.get_bool("allow-unordered", &allow_unordered, false);
  delimiter = s->info.args.get("delimiter");
  max_keys = s->info.args.get("max-keys");
  op_ret = parse_max_keys();
  if (op_ret < 0) {
   return op_ret;
  }
  encoding_type = s->info.args.get("encoding-type");
  if (s->system_request) {
    s->info.args.get_bool("objs-container", &objs_container, false);
    const char *shard_id_str = s->info.env->get("HTTP_RGWX_SHARD_ID");
    if (shard_id_str) {  
      string err;
      shard_id = strict_strtol(shard_id_str, 10, &err);
      if (!err.empty()) {
        ldout(s->cct, 5) << "bad shard id specified: " << shard_id_str << dendl;
        return -EINVAL;
      }
    } else {
     shard_id = s->bucket_instance_shard_id;
    }
  }
  return 0;
}