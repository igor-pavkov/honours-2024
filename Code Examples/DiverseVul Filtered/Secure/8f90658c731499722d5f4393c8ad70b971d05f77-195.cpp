int RGWListBucket_ObjStore_S3::get_params()
{
  int ret = get_common_params();
  if (ret < 0) {
    return ret;
  }
  if (!list_versions) {
    marker = s->info.args.get("marker");
  } else {
    marker.name = s->info.args.get("key-marker");
    marker.instance = s->info.args.get("version-id-marker");
  }
  return 0;
}