    set<rgw_zone_id> get_zone_ids_from_names(rgw::sal::RGWRadosStore *store,
                                             const vector<string>& zone_names) const {
      set<rgw_zone_id> ids;

      for (auto& name : zone_names) {
        rgw_zone_id id;
        if (store->svc()->zone->find_zone_id_by_name(name, &id)) {
          ids.insert(std::move(id));
        }
      }

      return ids;
    }