RGWOp *RGWHandler_REST_Obj_S3::op_get()
{
  if (is_acl_op()) {
    return new RGWGetACLs_ObjStore_S3;
  } else if (s->info.args.exists("uploadId")) {
    return new RGWListMultipart_ObjStore_S3;
  } else if (s->info.args.exists("layout")) {
    return new RGWGetObjLayout_ObjStore_S3;
  } else if (is_tagging_op()) {
    return new RGWGetObjTags_ObjStore_S3;
  } else if (is_obj_retention_op()) {
    return new RGWGetObjRetention_ObjStore_S3;
  } else if (is_obj_legal_hold_op()) {
    return new RGWGetObjLegalHold_ObjStore_S3;
  }
  return get_obj_op(true);
}