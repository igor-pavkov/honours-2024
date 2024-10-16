static void csnmp_host_definition_destroy(void *arg) /* {{{ */
{
  host_definition_t *hd;

  hd = arg;

  if (hd == NULL)
    return;

  if (hd->name != NULL) {
    DEBUG("snmp plugin: Destroying host definition for host `%s'.", hd->name);
  }

  csnmp_host_close_session(hd);

  sfree(hd->name);
  sfree(hd->address);
  sfree(hd->community);
  sfree(hd->username);
  sfree(hd->auth_passphrase);
  sfree(hd->priv_passphrase);
  sfree(hd->context);
  sfree(hd->data_list);

  sfree(hd);
} /* }}} void csnmp_host_definition_destroy */