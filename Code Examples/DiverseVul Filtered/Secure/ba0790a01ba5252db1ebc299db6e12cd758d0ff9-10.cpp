int RGWPostObj_ObjStore_S3::get_encrypt_filter(
    std::unique_ptr<RGWPutObjDataProcessor>* filter, RGWPutObjDataProcessor* cb)
{
  int res = 0;
  std::unique_ptr<BlockCrypt> block_crypt;
  res = rgw_s3_prepare_encrypt(s, attrs, &parts, &block_crypt, crypt_http_responses);
  if (res == 0 && block_crypt != nullptr) {
    *filter = std::unique_ptr<RGWPutObj_BlockEncrypt>(
        new RGWPutObj_BlockEncrypt(s->cct, cb, std::move(block_crypt)));
  }
  else
    *filter = nullptr;
  return res;
}