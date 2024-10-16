static int megasas_get_ld_vf_affiliation(struct megasas_instance *instance,
	int initial)
{
	int retval;

	if (instance->PlasmaFW111)
		retval = megasas_get_ld_vf_affiliation_111(instance, initial);
	else
		retval = megasas_get_ld_vf_affiliation_12(instance, initial);
	return retval;
}