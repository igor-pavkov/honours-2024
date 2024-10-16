RGWOp *RGWHandler_REST_Bucket_S3::op_delete()
{
  if (is_tagging_op()) {
    return new RGWDeleteBucketTags_ObjStore_S3;
  } else if (is_cors_op()) {
    return new RGWDeleteCORS_ObjStore_S3;
  } else if(is_lc_op()) {
    return new RGWDeleteLC_ObjStore_S3;
  } else if(is_policy_op()) {
    return new RGWDeleteBucketPolicy;
  } else if (is_notification_op()) {
    return RGWHandler_REST_PSNotifs_S3::create_delete_op();
  } else if (is_replication_op()) {
    return new RGWDeleteBucketReplication_ObjStore_S3;
  } else if (is_block_public_access_op()) {
    return new RGWDeleteBucketPublicAccessBlock;
  }

  if (s->info.args.sub_resource_exists("website")) {
    if (!s->cct->_conf->rgw_enable_static_website) {
      return NULL;
    }
    return new RGWDeleteBucketWebsite_ObjStore_S3;
  }

  if (s->info.args.exists("mdsearch")) {
    return new RGWDelBucketMetaSearch_ObjStore_S3;
  }

  return new RGWDeleteBucket_ObjStore_S3;
}