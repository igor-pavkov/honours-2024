RGWOp *RGWHandler_REST_Service_S3::op_head()
{
  return new RGWListBuckets_ObjStore_S3;
}