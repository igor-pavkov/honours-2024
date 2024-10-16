    vector<string> get_zone_names_from_ids(rgw::sal::RGWRadosStore *store,
                                           const set<rgw_zone_id>& zone_ids) const {
      vector<string> names;

      for (auto& id : zone_ids) {
        RGWZone *zone;
        if (store->svc()->zone->find_zone(id, &zone)) {
          names.emplace_back(zone->name);
        }
      }

      return names;
    }