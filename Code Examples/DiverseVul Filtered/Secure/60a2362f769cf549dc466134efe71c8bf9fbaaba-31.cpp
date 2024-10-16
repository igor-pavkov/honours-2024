int regulator_bulk_enable(int num_consumers,
			  struct regulator_bulk_data *consumers)
{
	ASYNC_DOMAIN_EXCLUSIVE(async_domain);
	int i;
	int ret = 0;

	for (i = 0; i < num_consumers; i++) {
		if (consumers[i].consumer->always_on)
			consumers[i].ret = 0;
		else
			async_schedule_domain(regulator_bulk_enable_async,
					      &consumers[i], &async_domain);
	}

	async_synchronize_full_domain(&async_domain);

	/* If any consumer failed we need to unwind any that succeeded */
	for (i = 0; i < num_consumers; i++) {
		if (consumers[i].ret != 0) {
			ret = consumers[i].ret;
			goto err;
		}
	}

	return 0;

err:
	for (i = 0; i < num_consumers; i++) {
		if (consumers[i].ret < 0)
			pr_err("Failed to enable %s: %d\n", consumers[i].supply,
			       consumers[i].ret);
		else
			regulator_disable(consumers[i].consumer);
	}

	return ret;
}