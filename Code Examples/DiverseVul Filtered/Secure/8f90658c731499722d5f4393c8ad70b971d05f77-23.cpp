      bool is_valid(CephContext *cct) const {
        bool result = is_valid_status(status);
        if (!result) {
          ldout(cct, 5) << "NOTICE: bad status provided in DeleteMarkerReplication element (status=" << status << ")" << dendl;
        }
        return result;
      }