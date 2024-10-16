int RGWCopyObj_ObjStore_S3::get_params()
{
  if_mod = s->info.env->get("HTTP_X_AMZ_COPY_IF_MODIFIED_SINCE");
  if_unmod = s->info.env->get("HTTP_X_AMZ_COPY_IF_UNMODIFIED_SINCE");
  if_match = s->info.env->get("HTTP_X_AMZ_COPY_IF_MATCH");
  if_nomatch = s->info.env->get("HTTP_X_AMZ_COPY_IF_NONE_MATCH");

  src_tenant_name = s->src_tenant_name;
  src_bucket_name = s->src_bucket_name;
  src_object = s->src_object;
  dest_tenant_name = s->bucket.tenant;
  dest_bucket_name = s->bucket.name;
  dest_object = s->object.name;

  if (s->system_request) {
    source_zone = s->info.args.get(RGW_SYS_PARAM_PREFIX "source-zone");
    s->info.args.get_bool(RGW_SYS_PARAM_PREFIX "copy-if-newer", &copy_if_newer, false);
    if (!source_zone.empty()) {
      client_id = s->info.args.get(RGW_SYS_PARAM_PREFIX "client-id");
      op_id = s->info.args.get(RGW_SYS_PARAM_PREFIX "op-id");

      if (client_id.empty() || op_id.empty()) {
	ldout(s->cct, 0) <<
	  RGW_SYS_PARAM_PREFIX "client-id or "
	  RGW_SYS_PARAM_PREFIX "op-id were not provided, "
	  "required for intra-region copy"
			 << dendl;
	return -EINVAL;
      }
    }
  }

  copy_source = s->info.env->get("HTTP_X_AMZ_COPY_SOURCE");
  auto tmp_md_d = s->info.env->get("HTTP_X_AMZ_METADATA_DIRECTIVE");
  if (tmp_md_d) {
    if (strcasecmp(tmp_md_d, "COPY") == 0) {
      attrs_mod = RGWRados::ATTRSMOD_NONE;
    } else if (strcasecmp(tmp_md_d, "REPLACE") == 0) {
      attrs_mod = RGWRados::ATTRSMOD_REPLACE;
    } else if (!source_zone.empty()) {
      attrs_mod = RGWRados::ATTRSMOD_NONE; // default for intra-zone_group copy
    } else {
      s->err.message = "Unknown metadata directive.";
      ldout(s->cct, 0) << s->err.message << dendl;
      return -EINVAL;
    }
    md_directive = tmp_md_d;
  }

  if (source_zone.empty() &&
      (dest_tenant_name.compare(src_tenant_name) == 0) &&
      (dest_bucket_name.compare(src_bucket_name) == 0) &&
      (dest_object.compare(src_object.name) == 0) &&
      src_object.instance.empty() &&
      (attrs_mod != RGWRados::ATTRSMOD_REPLACE)) {
    /* can only copy object into itself if replacing attrs */
    s->err.message = "This copy request is illegal because it is trying to copy "
                     "an object to itself without changing the object's metadata, "
                     "storage class, website redirect location or encryption attributes.";
    ldout(s->cct, 0) << s->err.message << dendl;
    return -ERR_INVALID_REQUEST;
  }
  return 0;
}