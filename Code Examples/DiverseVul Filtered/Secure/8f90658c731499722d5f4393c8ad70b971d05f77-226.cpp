    void from_sync_policy_pipe(rgw::sal::RGWRadosStore *store,
                              const rgw_sync_bucket_pipes& pipe,
                              bool enabled) {
      id = pipe.id;
      status = (enabled ? "Enabled" : "Disabled");
      priority = pipe.params.priority;

      if (pipe.source.all_zones) {
        source.reset();
      } else if (pipe.source.zones) {
        source.emplace();
        source->zone_names = get_zone_names_from_ids(store, *pipe.source.zones);
      }

      if (!pipe.dest.all_zones &&
          pipe.dest.zones) {
        destination.zone_names = get_zone_names_from_ids(store, *pipe.dest.zones);
      }

      if (pipe.params.dest.acl_translation) {
        destination.acl_translation.emplace();
        destination.acl_translation->owner = pipe.params.dest.acl_translation->owner.to_str();
      }

      if (pipe.params.dest.storage_class) {
        destination.storage_class = *pipe.params.dest.storage_class;
      }

      if (pipe.dest.bucket) {
        destination.bucket = pipe.dest.bucket->get_key();
      }

      filter.emplace();
      filter->from_sync_pipe_filter(pipe.params.source.filter);

      if (filter->empty()) {
        filter.reset();
      }
    }