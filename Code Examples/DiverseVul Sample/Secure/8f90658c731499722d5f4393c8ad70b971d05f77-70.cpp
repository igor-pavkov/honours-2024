    int to_sync_policy_pipe(req_state *s, rgw::sal::RGWRadosStore *store,
                            rgw_sync_bucket_pipes *pipe,
                            bool *enabled) const {
      if (!is_valid(s->cct)) {
        return -EINVAL;
      }

      pipe->id = id;
      pipe->params.priority = priority;

      const auto& user_id = s->user->get_id();

      rgw_bucket_key dest_bk(user_id.tenant,
                             destination.bucket);

      if (source && !source->zone_names.empty()) {
        pipe->source.zones = get_zone_ids_from_names(store, source->zone_names);
      } else {
        pipe->source.set_all_zones(true);
      }
      if (!destination.zone_names.empty()) {
        pipe->dest.zones = get_zone_ids_from_names(store, destination.zone_names);
      } else {
        pipe->dest.set_all_zones(true);
      }
      pipe->dest.bucket.emplace(dest_bk);

      if (filter) {
        int r = filter->to_sync_pipe_filter(s->cct, &pipe->params.source.filter);
        if (r < 0) {
          return r;
        }
      }
      if (destination.acl_translation) {
        rgw_user u;
        u.tenant = user_id.tenant;
        u.from_str(destination.acl_translation->owner); /* explicit tenant will override tenant,
                                                           otherwise will inherit it from s->user */
        pipe->params.dest.acl_translation.emplace();
        pipe->params.dest.acl_translation->owner = u;
      }
      pipe->params.dest.storage_class = destination.storage_class;

      *enabled = (status == "Enabled");

      pipe->params.mode = rgw_sync_pipe_params::Mode::MODE_USER;
      pipe->params.user = user_id.to_str();

      return 0;
    }