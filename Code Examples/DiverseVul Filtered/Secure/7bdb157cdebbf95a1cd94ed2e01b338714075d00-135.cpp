static int swevent_hlist_get(void)
{
	int err, cpu, failed_cpu;

	mutex_lock(&pmus_lock);
	for_each_possible_cpu(cpu) {
		err = swevent_hlist_get_cpu(cpu);
		if (err) {
			failed_cpu = cpu;
			goto fail;
		}
	}
	mutex_unlock(&pmus_lock);
	return 0;
fail:
	for_each_possible_cpu(cpu) {
		if (cpu == failed_cpu)
			break;
		swevent_hlist_put_cpu(cpu);
	}
	mutex_unlock(&pmus_lock);
	return err;
}