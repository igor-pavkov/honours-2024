RGWOp* RGWHandler_REST_S3Website::op_head()
{
  return get_obj_op(false);
}