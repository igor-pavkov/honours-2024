static int csnmp_config_add_data_instance_prefix(data_definition_t *dd,
                                                 oconfig_item_t *ci) {
  int status;

  if (!dd->is_table) {
    WARNING("snmp plugin: data %s: InstancePrefix is ignored when `Table' "
            "is set to `false'.",
            dd->name);
    return (-1);
  }

  status = cf_util_get_string(ci, &dd->instance_prefix);
  return status;
} /* int csnmp_config_add_data_instance_prefix */