void RGWDeleteBucketReplication_ObjStore_S3::update_sync_policy(rgw_sync_policy_info *policy)
{
  policy->groups.erase(enabled_group_id);
  policy->groups.erase(disabled_group_id);
}