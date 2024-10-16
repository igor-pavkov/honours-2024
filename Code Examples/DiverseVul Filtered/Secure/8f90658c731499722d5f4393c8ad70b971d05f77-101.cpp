RGWOp* RGWHandler_REST_S3Website::op_get()
{
  return get_obj_op(true);
}