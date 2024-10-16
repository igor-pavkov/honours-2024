int RGWGetObj_ObjStore_S3::get_decrypt_filter(std::unique_ptr<RGWGetObj_Filter> *filter, RGWGetObj_Filter* cb, bufferlist* manifest_bl)
{
  if (skip_decrypt) { // bypass decryption for multisite sync requests
    return 0;
  }

  int res = 0;
  std::unique_ptr<BlockCrypt> block_crypt;
  res = rgw_s3_prepare_decrypt(s, attrs, &block_crypt, crypt_http_responses);
  if (res == 0) {
    if (block_crypt != nullptr) {
      auto f = std::make_unique<RGWGetObj_BlockDecrypt>(s->cct, cb, std::move(block_crypt));
      if (manifest_bl != nullptr) {
        res = f->read_manifest(*manifest_bl);
        if (res == 0) {
          *filter = std::move(f);
        }
      }
    }
  }
  return res;
}