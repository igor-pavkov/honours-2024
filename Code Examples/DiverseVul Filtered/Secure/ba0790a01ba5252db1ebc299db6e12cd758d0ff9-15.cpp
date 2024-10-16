RGWOp *RGWHandler_REST_Bucket_S3::get_obj_op(bool get_data)
{
  // Non-website mode
  if (get_data) {
    return new RGWListBucket_ObjStore_S3;
  } else {
    return new RGWStatBucket_ObjStore_S3;
  }
}