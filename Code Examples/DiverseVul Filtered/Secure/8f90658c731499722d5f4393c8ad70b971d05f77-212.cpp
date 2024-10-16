int RGWGetObj_ObjStore_S3::get_params()
{
  // for multisite sync requests, only read the slo manifest itself, rather than
  // all of the data from its parts. the parts will sync as separate objects
  skip_manifest = s->info.args.exists(RGW_SYS_PARAM_PREFIX "sync-manifest");

  // multisite sync requests should fetch encrypted data, along with the
  // attributes needed to support decryption on the other zone
  if (s->system_request) {
    skip_decrypt = s->info.args.exists(RGW_SYS_PARAM_PREFIX "skip-decrypt");
  }

  return RGWGetObj_ObjStore::get_params();
}