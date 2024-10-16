static int create_s3_policy(struct req_state *s, rgw::sal::RGWRadosStore *store,
			    RGWAccessControlPolicy_S3& s3policy,
			    ACLOwner& owner)
{
  if (s->has_acl_header) {
    if (!s->canned_acl.empty())
      return -ERR_INVALID_REQUEST;

    return s3policy.create_from_headers(store->ctl()->user, s->info.env, owner);
  }

  return s3policy.create_canned(owner, s->bucket_owner, s->canned_acl);
}