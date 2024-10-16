RGWOp *RGWHandler_REST_Obj_S3::op_delete()
{
  if (is_tagging_op()) {
    return new RGWDeleteObjTags_ObjStore_S3;
  }
  string upload_id = s->info.args.get("uploadId");

  if (upload_id.empty())
    return new RGWDeleteObj_ObjStore_S3;
  else
    return new RGWAbortMultipart_ObjStore_S3;
}