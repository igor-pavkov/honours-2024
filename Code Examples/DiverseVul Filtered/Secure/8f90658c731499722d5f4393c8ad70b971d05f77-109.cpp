RGWOp *RGWHandler_REST_Bucket_S3::op_get()
{
  if (s->info.args.sub_resource_exists("logging"))
    return new RGWGetBucketLogging_ObjStore_S3;

  if (s->info.args.sub_resource_exists("location"))
    return new RGWGetBucketLocation_ObjStore_S3;

  if (s->info.args.sub_resource_exists("versioning"))
    return new RGWGetBucketVersioning_ObjStore_S3;

  if (s->info.args.sub_resource_exists("website")) {
    if (!s->cct->_conf->rgw_enable_static_website) {
      return NULL;
    }
    return new RGWGetBucketWebsite_ObjStore_S3;
  }

  if (s->info.args.exists("mdsearch")) {
    return new RGWGetBucketMetaSearch_ObjStore_S3;
  }

  if (is_acl_op()) {
    return new RGWGetACLs_ObjStore_S3;
  } else if (is_cors_op()) {
    return new RGWGetCORS_ObjStore_S3;
  } else if (is_request_payment_op()) {
    return new RGWGetRequestPayment_ObjStore_S3;
  } else if (s->info.args.exists("uploads")) {
    return new RGWListBucketMultiparts_ObjStore_S3;
  } else if(is_lc_op()) {
    return new RGWGetLC_ObjStore_S3;
  } else if(is_policy_op()) {
    return new RGWGetBucketPolicy;
  } else if (is_tagging_op()) {
    return new RGWGetBucketTags_ObjStore_S3;
  } else if (is_object_lock_op()) {
    return new RGWGetBucketObjectLock_ObjStore_S3;
  } else if (is_notification_op()) {
    return RGWHandler_REST_PSNotifs_S3::create_get_op();
  } else if (is_replication_op()) {
    return new RGWGetBucketReplication_ObjStore_S3;
  } else if (is_policy_status_op()) {
    return new RGWGetBucketPolicyStatus_ObjStore_S3;
  } else if (is_block_public_access_op()) {
    return new RGWGetBucketPublicAccessBlock_ObjStore_S3;
  }
  return get_obj_op(true);
}