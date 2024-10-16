RGWOp *RGWHandler_REST_Obj_S3::op_put()
{
  if (is_acl_op()) {
    return new RGWPutACLs_ObjStore_S3;
  } else if (is_tagging_op()) {
    return new RGWPutObjTags_ObjStore_S3;
  } else if (is_obj_retention_op()) {
    return new RGWPutObjRetention_ObjStore_S3;
  } else if (is_obj_legal_hold_op()) {
    return new RGWPutObjLegalHold_ObjStore_S3;
  }

  if (s->init_state.src_bucket.empty())
    return new RGWPutObj_ObjStore_S3;
  else
    return new RGWCopyObj_ObjStore_S3;
}