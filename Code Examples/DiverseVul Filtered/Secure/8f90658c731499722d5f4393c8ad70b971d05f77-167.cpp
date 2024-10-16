RGWOp *RGWHandler_REST_Bucket_S3::op_post()
{
  if (s->info.args.exists("delete")) {
    return new RGWDeleteMultiObj_ObjStore_S3;
  }

  if (s->info.args.exists("mdsearch")) {
    return new RGWConfigBucketMetaSearch_ObjStore_S3;
  }

  return new RGWPostObj_ObjStore_S3;
}