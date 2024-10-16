static struct megasas_instance *megasas_lookup_instance(u16 host_no)
{
	int i;

	for (i = 0; i < megasas_mgmt_info.max_index; i++) {

		if ((megasas_mgmt_info.instance[i]) &&
		    (megasas_mgmt_info.instance[i]->host->host_no == host_no))
			return megasas_mgmt_info.instance[i];
	}

	return NULL;
}