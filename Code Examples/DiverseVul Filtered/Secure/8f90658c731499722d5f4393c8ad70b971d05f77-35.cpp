  void decode_xml(XMLObj *obj) {
    string status_str;
    string mfa_str;
    RGWXMLDecoder::decode_xml("Status", status_str, obj);
    if (status_str == "Enabled") {
      status = VersioningEnabled;
    } else if (status_str != "Suspended") {
      status = VersioningStatusInvalid;
    }


    if (RGWXMLDecoder::decode_xml("MfaDelete", mfa_str, obj)) {
      if (mfa_str == "Enabled") {
        mfa_status = MFA_ENABLED;
      } else if (mfa_str == "Disabled") {
        mfa_status = MFA_DISABLED;
      } else {
        retcode = -EINVAL;
      }
    }
  }