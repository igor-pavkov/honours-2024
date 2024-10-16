uschar *dkim_exim_expand_defaults(int what) {
  switch(what) {
    case DKIM_ALGO:               return US"";
    case DKIM_BODYLENGTH:         return US"9999999999999";
    case DKIM_CANON_BODY:         return US"";
    case DKIM_CANON_HEADERS:      return US"";
    case DKIM_COPIEDHEADERS:      return US"";
    case DKIM_CREATED:            return US"0";
    case DKIM_EXPIRES:            return US"9999999999999";
    case DKIM_HEADERNAMES:        return US"";
    case DKIM_IDENTITY:           return US"";
    case DKIM_KEY_GRANULARITY:    return US"*";
    case DKIM_KEY_SRVTYPE:        return US"*";
    case DKIM_KEY_NOTES:          return US"";
    case DKIM_KEY_TESTING:        return US"0";
    case DKIM_NOSUBDOMAINS:       return US"0";
    case DKIM_VERIFY_STATUS:      return US"none";
    case DKIM_VERIFY_REASON:      return US"";
    default:                      return US"";
  }
}