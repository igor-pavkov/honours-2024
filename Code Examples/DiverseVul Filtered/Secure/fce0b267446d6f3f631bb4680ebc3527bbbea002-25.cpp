int RGWHandler_REST_S3::postauth_init()
{
  struct req_init_state *t = &s->init_state;
  bool relaxed_names = s->cct->_conf->rgw_relaxed_s3_bucket_names;

  rgw_parse_url_bucket(t->url_bucket, s->user->user_id.tenant,
		      s->bucket_tenant, s->bucket_name);

  dout(10) << "s->object=" << (!s->object.empty() ? s->object : rgw_obj_key("<NULL>"))
           << " s->bucket=" << rgw_make_bucket_entry_name(s->bucket_tenant, s->bucket_name) << dendl;

  int ret;
  ret = rgw_validate_tenant_name(s->bucket_tenant);
  if (ret)
    return ret;
  if (!s->bucket_name.empty()) {
    ret = valid_s3_bucket_name(s->bucket_name, relaxed_names);
    if (ret)
      return ret;
    ret = validate_object_name(s->object.name);
    if (ret)
      return ret;
  }

  if (!t->src_bucket.empty()) {
    rgw_parse_url_bucket(t->src_bucket, s->user->user_id.tenant,
			s->src_tenant_name, s->src_bucket_name);
    ret = rgw_validate_tenant_name(s->src_tenant_name);
    if (ret)
      return ret;
    ret = valid_s3_bucket_name(s->src_bucket_name, relaxed_names);
    if (ret)
      return ret;
  }

  const char *mfa = s->info.env->get("HTTP_X_AMZ_MFA");
  if (mfa) {
    ret = verify_mfa(store, s->user, string(mfa), &s->mfa_verified);
  }

  return 0;
}