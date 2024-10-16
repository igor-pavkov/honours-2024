void RGWListBuckets_ObjStore_S3::send_response_data(rgw::sal::RGWBucketList& buckets)
{
  if (!sent_data)
    return;

  map<string, rgw::sal::RGWBucket*>& m = buckets.get_buckets();
  map<string, rgw::sal::RGWBucket*>::iterator iter;

  for (iter = m.begin(); iter != m.end(); ++iter) {
    rgw::sal::RGWBucket* obj = iter->second;
    dump_bucket(s, *obj);
  }
  rgw_flush_formatter(s, s->formatter);
}