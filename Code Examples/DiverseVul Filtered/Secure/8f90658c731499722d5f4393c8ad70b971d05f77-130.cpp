RGWOp *RGWHandler_REST_Obj_S3::get_obj_op(bool get_data)
{
  RGWGetObj_ObjStore_S3 *get_obj_op = new RGWGetObj_ObjStore_S3;
  get_obj_op->set_get_data(get_data);
  return get_obj_op;
}