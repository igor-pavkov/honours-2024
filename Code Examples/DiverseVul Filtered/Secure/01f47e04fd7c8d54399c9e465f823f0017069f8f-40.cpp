void Lua::setInterface(const char *user) {
  char key[64], ifname[MAX_INTERFACE_NAME_LEN];
  bool enforce_allowed_interface = false;

  if(user[0] != '\0') {
    // check if the user is restricted to browse only a given interface

    if(snprintf(key, sizeof(key), CONST_STR_USER_ALLOWED_IFNAME, user)
       && !ntop->getRedis()->get(key, ifname, sizeof(ifname))) {
      // there is only one allowed interface for the user
      enforce_allowed_interface = true;
      goto set_preferred_interface;
    } else if(snprintf(key, sizeof(key), "ntopng.prefs.%s.ifname", user)
	      && ntop->getRedis()->get(key, ifname, sizeof(ifname)) < 0) {
      // no allowed interface and no default set interface
    set_default_if_name_in_session:
      snprintf(ifname, sizeof(ifname), "%s",
	       ntop->getInterfaceAtId(NULL /* allowed user interface check already enforced */,
				      0)->get_name());
      lua_push_str_table_entry(L, "ifname", ifname);
      ntop->getRedis()->set(key, ifname, 3600 /* 1h */);
    } else {
      goto set_preferred_interface;
    }
  } else {
    // We need to check if ntopng is running with the option --disable-login
    snprintf(key, sizeof(key), "ntopng.prefs.ifname");
    if(ntop->getRedis()->get(key, ifname, sizeof(ifname)) < 0) {
      goto set_preferred_interface;
    }

  set_preferred_interface:
    NetworkInterface *iface;

    if((iface = ntop->getNetworkInterface(NULL /* allowed user interface check already enforced */,
					      ifname)) != NULL) {
      /* The specified interface still exists */
      lua_push_str_table_entry(L, "ifname", iface->get_name());
    } else if(!enforce_allowed_interface) {
      goto set_default_if_name_in_session;
    } else {
      // TODO: handle the case where the user has
      // an allowed interface that is not presently available
      // (e.g., not running?)
    }
  }
}