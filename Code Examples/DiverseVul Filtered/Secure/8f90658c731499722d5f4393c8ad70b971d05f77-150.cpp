  int to_sync_policy_groups(req_state *s, rgw::sal::RGWRadosStore *store,
                            vector<rgw_sync_policy_group> *result) const {
    result->resize(2);

    rgw_sync_policy_group& enabled_group = (*result)[0];
    rgw_sync_policy_group& disabled_group = (*result)[1];

    enabled_group.id = enabled_group_id;
    enabled_group.status = rgw_sync_policy_group::Status::ENABLED;
    disabled_group.id = disabled_group_id;
    disabled_group.status = rgw_sync_policy_group::Status::ALLOWED; /* not enabled, not forbidden */

    for (auto& rule : rules) {
      rgw_sync_bucket_pipes pipe;
      bool enabled;
      int r = rule.to_sync_policy_pipe(s, store, &pipe, &enabled);
      if (r < 0) {
        ldout(s->cct, 5) << "NOTICE: failed to convert replication configuration into sync policy pipe (rule.id=" << rule.id << "): " << cpp_strerror(-r) << dendl;
        return r;
      }

      if (enabled) {
        enabled_group.pipes.emplace_back(std::move(pipe));
      } else {
        disabled_group.pipes.emplace_back(std::move(pipe));
      }
    }
    return 0;
  }