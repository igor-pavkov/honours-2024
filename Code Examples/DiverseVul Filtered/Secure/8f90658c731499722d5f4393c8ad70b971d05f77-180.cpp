RGWOp *RGWHandler_REST_Bucket_S3::op_options()
{
  return new RGWOptionsCORS_ObjStore_S3;
}