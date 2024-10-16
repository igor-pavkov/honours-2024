static void sas_revalidate_domain(struct work_struct *work)
{
	int res = 0;
	struct sas_discovery_event *ev = to_sas_discovery_event(work);
	struct asd_sas_port *port = ev->port;
	struct sas_ha_struct *ha = port->ha;
	struct domain_device *ddev = port->port_dev;

	/* prevent revalidation from finding sata links in recovery */
	mutex_lock(&ha->disco_mutex);
	if (test_bit(SAS_HA_ATA_EH_ACTIVE, &ha->state)) {
		pr_debug("REVALIDATION DEFERRED on port %d, pid:%d\n",
			 port->id, task_pid_nr(current));
		goto out;
	}

	clear_bit(DISCE_REVALIDATE_DOMAIN, &port->disc.pending);

	pr_debug("REVALIDATING DOMAIN on port %d, pid:%d\n", port->id,
		 task_pid_nr(current));

	if (ddev && dev_is_expander(ddev->dev_type))
		res = sas_ex_revalidate_domain(ddev);

	pr_debug("done REVALIDATING DOMAIN on port %d, pid:%d, res 0x%x\n",
		 port->id, task_pid_nr(current), res);
 out:
	mutex_unlock(&ha->disco_mutex);

	sas_destruct_devices(port);
	sas_destruct_ports(port);
	sas_probe_devices(port);
}