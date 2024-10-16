RGWOp *RGWHandler_REST_Bucket_S3::op_put()
{
  if (s->info.args.sub_resource_exists("logging"))
    return NULL;
  if (s->info.args.sub_resource_exists("versioning"))
    return new RGWSetBucketVersioning_ObjStore_S3;
  if (s->info.args.sub_resource_exists("website")) {
    if (!s->cct->_conf->rgw_enable_static_website) {
      return NULL;
    }
    return new RGWSetBucketWebsite_ObjStore_S3;
  }
  if (is_tagging_op()) {
    return new RGWPutBucketTags_ObjStore_S3;
  } else if (is_acl_op()) {
    return new RGWPutACLs_ObjStore_S3;
  } else if (is_cors_op()) {
    return new RGWPutCORS_ObjStore_S3;
  } else if (is_request_payment_op()) {
    return new RGWSetRequestPayment_ObjStore_S3;
  } else if(is_lc_op()) {
    return new RGWPutLC_ObjStore_S3;
  } else if(is_policy_op()) {
    return new RGWPutBucketPolicy;
  } else if (is_object_lock_op()) {
    return new RGWPutBucketObjectLock_ObjStore_S3;
  } else if (is_notification_op()) {
    return RGWHandler_REST_PSNotifs_S3::create_put_op();
  } else if (is_replication_op()) {
    auto sync_policy_handler = store->svc()->zone->get_sync_policy_handler(nullopt);
    if (!sync_policy_handler ||
        sync_policy_handler->is_legacy_config()) {
      return nullptr;
    }

    return new RGWPutBucketReplication_ObjStore_S3;
  } else if (is_block_public_access_op()) {
    return new RGWPutBucketPublicAccessBlock_ObjStore_S3;
  }
  return new RGWCreateBucket_ObjStore_S3;
}