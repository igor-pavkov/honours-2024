int RGWPutObj_ObjStore_S3::get_data(bufferlist& bl)
{
  const int ret = RGWPutObj_ObjStore::get_data(bl);
  if (ret == 0) {
    const int ret_auth = do_aws4_auth_completion();
    if (ret_auth < 0) {
      return ret_auth;
    }
  }

  return ret;
}