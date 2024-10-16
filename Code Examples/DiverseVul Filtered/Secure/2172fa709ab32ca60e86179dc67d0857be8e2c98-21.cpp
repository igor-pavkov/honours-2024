static void security_load_policycaps(void)
{
	selinux_policycap_netpeer = ebitmap_get_bit(&policydb.policycaps,
						  POLICYDB_CAPABILITY_NETPEER);
	selinux_policycap_openperm = ebitmap_get_bit(&policydb.policycaps,
						  POLICYDB_CAPABILITY_OPENPERM);
	selinux_policycap_alwaysnetwork = ebitmap_get_bit(&policydb.policycaps,
						  POLICYDB_CAPABILITY_ALWAYSNETWORK);
}