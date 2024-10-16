  virtual bool gate (function *)
    {
      /* FIXME: We should remove the optimize check after we ensure we never run
	 IPA passes when not optimizing.  */
      return (flag_ipa_cp && optimize) || in_lto_p;
    }