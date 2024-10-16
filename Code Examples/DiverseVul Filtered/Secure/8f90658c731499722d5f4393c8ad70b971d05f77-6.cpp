      int to_sync_pipe_filter(CephContext *cct,
                              rgw_sync_pipe_filter *f) const {
        if (!is_valid(cct)) {
          return -EINVAL;
        }
        if (prefix) {
          f->prefix = *prefix;
        }
        if (tag) {
          f->tags.insert(rgw_sync_pipe_filter_tag(tag->key, tag->value));
        }

        if (and_elements) {
          if (and_elements->prefix) {
            f->prefix = *and_elements->prefix;
          }
          for (auto& t : and_elements->tags) {
            f->tags.insert(rgw_sync_pipe_filter_tag(t.key, t.value));
          }
        }
        return 0;
      }