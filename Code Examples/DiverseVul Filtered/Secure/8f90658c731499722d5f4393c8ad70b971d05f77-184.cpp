int RGWPutObj_ObjStore_S3::get_encrypt_filter(
    std::unique_ptr<rgw::putobj::DataProcessor> *filter,
    rgw::putobj::DataProcessor *cb)
{
  int res = 0;
  if (!multipart_upload_id.empty()) {
    RGWMPObj mp(s->object.name, multipart_upload_id);
    rgw_obj obj;
    obj.init_ns(s->bucket, mp.get_meta(), RGW_OBJ_NS_MULTIPART);
    obj.set_in_extra_data(true);
    map<string, bufferlist> xattrs;
    res = get_obj_attrs(store, s, obj, xattrs);
    if (res == 0) {
      std::unique_ptr<BlockCrypt> block_crypt;
      /* We are adding to existing object.
       * We use crypto mode that configured as if we were decrypting. */
      res = rgw_s3_prepare_decrypt(s, xattrs, &block_crypt, crypt_http_responses);
      if (res == 0 && block_crypt != nullptr)
        filter->reset(new RGWPutObj_BlockEncrypt(s->cct, cb, std::move(block_crypt)));
    }
    /* it is ok, to not have encryption at all */
  }
  else
  {
    std::unique_ptr<BlockCrypt> block_crypt;
    res = rgw_s3_prepare_encrypt(s, attrs, nullptr, &block_crypt, crypt_http_responses);
    if (res == 0 && block_crypt != nullptr) {
      filter->reset(new RGWPutObj_BlockEncrypt(s->cct, cb, std::move(block_crypt)));
    }
  }
  return res;
}