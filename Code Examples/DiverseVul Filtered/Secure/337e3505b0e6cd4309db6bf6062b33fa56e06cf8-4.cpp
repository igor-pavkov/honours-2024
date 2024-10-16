uschar *dkim_exim_expand_query(int what) {

  if (!dkim_verify_ctx ||
      dkim_disable_verify ||
      !dkim_cur_sig) return dkim_exim_expand_defaults(what);

  switch(what) {
    case DKIM_ALGO:
      switch(dkim_cur_sig->algo) {
        case PDKIM_ALGO_RSA_SHA1:
          return US"rsa-sha1";
        case PDKIM_ALGO_RSA_SHA256:
        default:
          return US"rsa-sha256";
      }
    case DKIM_BODYLENGTH:
      return (dkim_cur_sig->bodylength >= 0)?
              (uschar *)string_sprintf(OFF_T_FMT,(LONGLONG_T)dkim_cur_sig->bodylength)
              :dkim_exim_expand_defaults(what);
    case DKIM_CANON_BODY:
      switch(dkim_cur_sig->canon_body) {
        case PDKIM_CANON_RELAXED:
          return US"relaxed";
        case PDKIM_CANON_SIMPLE:
        default:
          return US"simple";
      }
    case DKIM_CANON_HEADERS:
      switch(dkim_cur_sig->canon_headers) {
        case PDKIM_CANON_RELAXED:
          return US"relaxed";
        case PDKIM_CANON_SIMPLE:
        default:
          return US"simple";
      }
    case DKIM_COPIEDHEADERS:
      return dkim_cur_sig->copiedheaders?
              (uschar *)(dkim_cur_sig->copiedheaders)
              :dkim_exim_expand_defaults(what);
    case DKIM_CREATED:
      return (dkim_cur_sig->created > 0)?
              (uschar *)string_sprintf("%llu",dkim_cur_sig->created)
              :dkim_exim_expand_defaults(what);
    case DKIM_EXPIRES:
      return (dkim_cur_sig->expires > 0)?
              (uschar *)string_sprintf("%llu",dkim_cur_sig->expires)
              :dkim_exim_expand_defaults(what);
    case DKIM_HEADERNAMES:
      return dkim_cur_sig->headernames?
              (uschar *)(dkim_cur_sig->headernames)
              :dkim_exim_expand_defaults(what);
    case DKIM_IDENTITY:
      return dkim_cur_sig->identity?
              (uschar *)(dkim_cur_sig->identity)
              :dkim_exim_expand_defaults(what);
    case DKIM_KEY_GRANULARITY:
      return dkim_cur_sig->pubkey?
              (dkim_cur_sig->pubkey->granularity?
                (uschar *)(dkim_cur_sig->pubkey->granularity)
                :dkim_exim_expand_defaults(what)
              )
              :dkim_exim_expand_defaults(what);
    case DKIM_KEY_SRVTYPE:
      return dkim_cur_sig->pubkey?
              (dkim_cur_sig->pubkey->srvtype?
                (uschar *)(dkim_cur_sig->pubkey->srvtype)
                :dkim_exim_expand_defaults(what)
              )
              :dkim_exim_expand_defaults(what);
    case DKIM_KEY_NOTES:
      return dkim_cur_sig->pubkey?
              (dkim_cur_sig->pubkey->notes?
                (uschar *)(dkim_cur_sig->pubkey->notes)
                :dkim_exim_expand_defaults(what)
              )
              :dkim_exim_expand_defaults(what);
    case DKIM_KEY_TESTING:
      return dkim_cur_sig->pubkey?
              (dkim_cur_sig->pubkey->testing?
                US"1"
                :dkim_exim_expand_defaults(what)
              )
              :dkim_exim_expand_defaults(what);
    case DKIM_NOSUBDOMAINS:
      return dkim_cur_sig->pubkey?
              (dkim_cur_sig->pubkey->no_subdomaining?
                US"1"
                :dkim_exim_expand_defaults(what)
              )
              :dkim_exim_expand_defaults(what);
    case DKIM_VERIFY_STATUS:
      switch(dkim_cur_sig->verify_status) {
        case PDKIM_VERIFY_INVALID:
          return US"invalid";
        case PDKIM_VERIFY_FAIL:
          return US"fail";
        case PDKIM_VERIFY_PASS:
          return US"pass";
        case PDKIM_VERIFY_NONE:
        default:
          return US"none";
      }
    case DKIM_VERIFY_REASON:
      switch (dkim_cur_sig->verify_ext_status) {
        case PDKIM_VERIFY_INVALID_PUBKEY_UNAVAILABLE:
          return US"pubkey_unavailable";
        case PDKIM_VERIFY_INVALID_PUBKEY_PARSING:
          return US"pubkey_syntax";
        case PDKIM_VERIFY_FAIL_BODY:
          return US"bodyhash_mismatch";
        case PDKIM_VERIFY_FAIL_MESSAGE:
          return US"signature_incorrect";
      }
    default:
      return US"";
  }
}