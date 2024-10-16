int RGWInitMultipart_ObjStore_S3::prepare_encryption(map<string, bufferlist>& attrs)
{
  int res = 0;
  res = rgw_s3_prepare_encrypt(s, attrs, nullptr, nullptr, crypt_http_responses);
  return res;
}