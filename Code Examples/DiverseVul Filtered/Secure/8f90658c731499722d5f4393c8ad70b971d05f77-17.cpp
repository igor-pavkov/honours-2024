void RGWGetBucketReplication_ObjStore_S3::send_response_data()
{
  if (op_ret)
    set_req_state_err(s, op_ret);
  dump_errno(s);
  end_header(s, this, "application/xml");
  dump_start(s);

  ReplicationConfiguration conf;

  if (s->bucket_info.sync_policy) {
    auto policy = s->bucket_info.sync_policy;

    auto iter = policy->groups.find(enabled_group_id);
    if (iter != policy->groups.end()) {
      conf.from_sync_policy_group(store, iter->second);
    }
    iter = policy->groups.find(disabled_group_id);
    if (iter != policy->groups.end()) {
      conf.from_sync_policy_group(store, iter->second);
    }
  }

  if (!op_ret) {
  s->formatter->open_object_section_in_ns("ReplicationConfiguration", XMLNS_AWS_S3);
  conf.dump_xml(s->formatter);
  s->formatter->close_section();
  rgw_flush_formatter_and_reset(s, s->formatter);
  }
}