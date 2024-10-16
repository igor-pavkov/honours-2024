      void from_sync_pipe_filter(const rgw_sync_pipe_filter& f) {
        if (f.prefix && f.tags.empty()) {
          prefix = f.prefix;
          return;
        }
        if (f.prefix) {
          and_elements.emplace();
          and_elements->prefix = f.prefix;
        } else if (f.tags.size() == 1) {
          auto iter = f.tags.begin();
          if (iter == f.tags.end()) {
            /* should never happen */
            return;
          }
          auto& t = *iter;
          tag.emplace();
          tag->key = t.key;
          tag->value = t.value;
          return;
        }

        if (f.tags.empty()) {
          return;
        }

        if (!and_elements) {
          and_elements.emplace();
        }

        for (auto& t : f.tags) {
          auto& tag = and_elements->tags.emplace_back();
          tag.key = t.key;
          tag.value = t.value;
        }
      }