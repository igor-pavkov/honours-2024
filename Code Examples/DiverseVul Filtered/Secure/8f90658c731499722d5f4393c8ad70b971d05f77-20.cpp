int RGWListBucket_ObjStore_S3v2::get_params()
{
int ret = get_common_params();
if (ret < 0) {
  return ret;
}
s->info.args.get_bool("fetch-owner", &fetchOwner, false);
startAfter = s->info.args.get("start-after", &start_after_exist);
continuation_token = s->info.args.get("continuation-token", &continuation_token_exist);
if(!continuation_token_exist) {
  marker = startAfter;
} else {
  marker = continuation_token;
}
return 0;
}