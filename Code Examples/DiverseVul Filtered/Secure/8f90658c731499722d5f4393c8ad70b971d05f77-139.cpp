      bool is_valid(CephContext *cct) const {
        if (tag && prefix) {
          ldout(cct, 5) << "NOTICE: both tag and prefix were provided in replication filter rule" << dendl;
          return false;
        }

        if (and_elements) {
          if (prefix && and_elements->prefix) {
            ldout(cct, 5) << "NOTICE: too many prefixes were provided in re" << dendl;
            return false;
          }
        }
        return true;
      };