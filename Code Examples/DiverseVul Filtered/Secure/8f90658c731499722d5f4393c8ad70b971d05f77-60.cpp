int RGWPostObj_ObjStore_S3::get_encrypt_filter(
    std::unique_ptr<rgw::putobj::DataProcessor> *filter,
    rgw::putobj::DataProcessor *cb)
{
  std::unique_ptr<BlockCrypt> block_crypt;
  int res = rgw_s3_prepare_encrypt(s, attrs, &parts, &block_crypt,
                                   crypt_http_responses);
  if (res == 0 && block_crypt != nullptr) {
    filter->reset(new RGWPutObj_BlockEncrypt(s->cct, cb, std::move(block_crypt)));
  }
  return res;
}