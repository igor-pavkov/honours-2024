RGWOp *RGWHandler_REST_Obj_S3::op_options()
{
  return new RGWOptionsCORS_ObjStore_S3;
}