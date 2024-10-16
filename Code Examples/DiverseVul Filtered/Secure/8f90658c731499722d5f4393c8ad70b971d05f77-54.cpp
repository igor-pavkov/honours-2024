  void from_sync_policy_group(rgw::sal::RGWRadosStore *store,
                              const rgw_sync_policy_group& group) {

    bool enabled = (group.status == rgw_sync_policy_group::Status::ENABLED);

    for (auto& pipe : group.pipes) {
      auto& rule = rules.emplace_back();
      rule.from_sync_policy_pipe(store, pipe, enabled);
    }
  }