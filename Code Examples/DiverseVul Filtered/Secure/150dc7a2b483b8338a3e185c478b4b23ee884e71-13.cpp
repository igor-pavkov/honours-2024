static void run_script(const char *action, double offset)
{
	char *argv[3];
	char *env1, *env2, *env3, *env4;

	G.last_script_run = G.cur_time;

	if (!G.script_name)
		return;

	argv[0] = (char*) G.script_name;
	argv[1] = (char*) action;
	argv[2] = NULL;

	VERB1 bb_error_msg("executing '%s %s'", G.script_name, action);

	env1 = xasprintf("%s=%u", "stratum", G.stratum);
	putenv(env1);
	env2 = xasprintf("%s=%ld", "freq_drift_ppm", G.kernel_freq_drift);
	putenv(env2);
	env3 = xasprintf("%s=%u", "poll_interval", 1 << G.poll_exp);
	putenv(env3);
	env4 = xasprintf("%s=%f", "offset", offset);
	putenv(env4);
	/* Other items of potential interest: selected peer,
	 * rootdelay, reftime, rootdisp, refid, ntp_status,
	 * last_update_offset, last_update_recv_time, discipline_jitter,
	 * how many peers have reachable_bits = 0?
	 */

	/* Don't want to wait: it may run hwclock --systohc, and that
	 * may take some time (seconds): */
	/*spawn_and_wait(argv);*/
	spawn(argv);

	unsetenv("stratum");
	unsetenv("freq_drift_ppm");
	unsetenv("poll_interval");
	unsetenv("offset");
	free(env1);
	free(env2);
	free(env3);
	free(env4);
}