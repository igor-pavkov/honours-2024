    bool is_valid(CephContext *cct) const {
      if (!is_valid_status(status)) {
        ldout(cct, 5) << "NOTICE: bad status provided in rule (status=" << status << ")" << dendl;
        return false;
      }
      if ((filter && !filter->is_valid(cct)) ||
          (delete_marker_replication && !delete_marker_replication->is_valid(cct))) {
        return false;
      }
      return true;
    }