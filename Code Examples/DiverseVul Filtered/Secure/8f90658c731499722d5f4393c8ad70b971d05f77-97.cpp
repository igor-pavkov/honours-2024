int RGWInitMultipart_ObjStore_S3::get_params()
{
  RGWAccessControlPolicy_S3 s3policy(s->cct);
  op_ret = create_s3_policy(s, store, s3policy, s->owner);
  if (op_ret < 0)
    return op_ret;

  policy = s3policy;

  return 0;
}