int RGWPutObj_ObjStore_S3::get_decrypt_filter(
    std::unique_ptr<RGWGetObj_Filter>* filter,
    RGWGetObj_Filter* cb,
    map<string, bufferlist>& attrs,
    bufferlist* manifest_bl)
{
  std::map<std::string, std::string> crypt_http_responses_unused;

  int res = 0;
  std::unique_ptr<BlockCrypt> block_crypt;
  res = rgw_s3_prepare_decrypt(s, attrs, &block_crypt, crypt_http_responses_unused);
  if (res == 0) {
    if (block_crypt != nullptr) {
      auto f = std::unique_ptr<RGWGetObj_BlockDecrypt>(new RGWGetObj_BlockDecrypt(s->cct, cb, std::move(block_crypt)));
      //RGWGetObj_BlockDecrypt* f = new RGWGetObj_BlockDecrypt(s->cct, cb, std::move(block_crypt));
      if (f != nullptr) {
        if (manifest_bl != nullptr) {
          res = f->read_manifest(*manifest_bl);
          if (res == 0) {
            *filter = std::move(f);
          }
        }
      }
    }
  }
  return res;
}