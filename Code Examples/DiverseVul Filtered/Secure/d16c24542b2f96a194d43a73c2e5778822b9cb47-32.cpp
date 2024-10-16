static int csnmp_config_add_host(oconfig_item_t *ci) {
  host_definition_t *hd;
  int status = 0;

  /* Registration stuff. */
  char cb_name[DATA_MAX_NAME_LEN];

  hd = calloc(1, sizeof(*hd));
  if (hd == NULL)
    return (-1);
  hd->version = 2;
  C_COMPLAIN_INIT(&hd->complaint);

  status = cf_util_get_string(ci, &hd->name);
  if (status != 0) {
    sfree(hd);
    return status;
  }

  hd->sess_handle = NULL;
  hd->interval = 0;

  for (int i = 0; i < ci->children_num; i++) {
    oconfig_item_t *option = ci->children + i;
    status = 0;

    if (strcasecmp("Address", option->key) == 0)
      status = cf_util_get_string(option, &hd->address);
    else if (strcasecmp("Community", option->key) == 0)
      status = cf_util_get_string(option, &hd->community);
    else if (strcasecmp("Version", option->key) == 0)
      status = csnmp_config_add_host_version(hd, option);
    else if (strcasecmp("Collect", option->key) == 0)
      csnmp_config_add_host_collect(hd, option);
    else if (strcasecmp("Interval", option->key) == 0)
      cf_util_get_cdtime(option, &hd->interval);
    else if (strcasecmp("Username", option->key) == 0)
      status = cf_util_get_string(option, &hd->username);
    else if (strcasecmp("AuthProtocol", option->key) == 0)
      status = csnmp_config_add_host_auth_protocol(hd, option);
    else if (strcasecmp("PrivacyProtocol", option->key) == 0)
      status = csnmp_config_add_host_priv_protocol(hd, option);
    else if (strcasecmp("AuthPassphrase", option->key) == 0)
      status = cf_util_get_string(option, &hd->auth_passphrase);
    else if (strcasecmp("PrivacyPassphrase", option->key) == 0)
      status = cf_util_get_string(option, &hd->priv_passphrase);
    else if (strcasecmp("SecurityLevel", option->key) == 0)
      status = csnmp_config_add_host_security_level(hd, option);
    else if (strcasecmp("Context", option->key) == 0)
      status = cf_util_get_string(option, &hd->context);
    else {
      WARNING(
          "snmp plugin: csnmp_config_add_host: Option `%s' not allowed here.",
          option->key);
      status = -1;
    }

    if (status != 0)
      break;
  } /* for (ci->children) */

  while (status == 0) {
    if (hd->address == NULL) {
      WARNING("snmp plugin: `Address' not given for host `%s'", hd->name);
      status = -1;
      break;
    }
    if (hd->community == NULL && hd->version < 3) {
      WARNING("snmp plugin: `Community' not given for host `%s'", hd->name);
      status = -1;
      break;
    }
    if (hd->version == 3) {
      if (hd->username == NULL) {
        WARNING("snmp plugin: `Username' not given for host `%s'", hd->name);
        status = -1;
        break;
      }
      if (hd->security_level == 0) {
        WARNING("snmp plugin: `SecurityLevel' not given for host `%s'",
                hd->name);
        status = -1;
        break;
      }
      if (hd->security_level == SNMP_SEC_LEVEL_AUTHNOPRIV ||
          hd->security_level == SNMP_SEC_LEVEL_AUTHPRIV) {
        if (hd->auth_protocol == NULL) {
          WARNING("snmp plugin: `AuthProtocol' not given for host `%s'",
                  hd->name);
          status = -1;
          break;
        }
        if (hd->auth_passphrase == NULL) {
          WARNING("snmp plugin: `AuthPassphrase' not given for host `%s'",
                  hd->name);
          status = -1;
          break;
        }
      }
      if (hd->security_level == SNMP_SEC_LEVEL_AUTHPRIV) {
        if (hd->priv_protocol == NULL) {
          WARNING("snmp plugin: `PrivacyProtocol' not given for host `%s'",
                  hd->name);
          status = -1;
          break;
        }
        if (hd->priv_passphrase == NULL) {
          WARNING("snmp plugin: `PrivacyPassphrase' not given for host `%s'",
                  hd->name);
          status = -1;
          break;
        }
      }
    }

    break;
  } /* while (status == 0) */

  if (status != 0) {
    csnmp_host_definition_destroy(hd);
    return (-1);
  }

  DEBUG("snmp plugin: hd = { name = %s, address = %s, community = %s, version "
        "= %i }",
        hd->name, hd->address, hd->community, hd->version);

  ssnprintf(cb_name, sizeof(cb_name), "snmp-%s", hd->name);

  user_data_t ud = {.data = hd, .free_func = csnmp_host_definition_destroy};

  status =
      plugin_register_complex_read(/* group = */ NULL, cb_name, csnmp_read_host,
                                   hd->interval, /* user_data = */ &ud);
  if (status != 0) {
    ERROR("snmp plugin: Registering complex read function failed.");
    csnmp_host_definition_destroy(hd);
    return (-1);
  }

  return (0);
} /* int csnmp_config_add_host */