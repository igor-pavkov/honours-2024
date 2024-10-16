void perf_event_ksymbol(u16 ksym_type, u64 addr, u32 len, bool unregister,
			const char *sym)
{
	struct perf_ksymbol_event ksymbol_event;
	char name[KSYM_NAME_LEN];
	u16 flags = 0;
	int name_len;

	if (!atomic_read(&nr_ksymbol_events))
		return;

	if (ksym_type >= PERF_RECORD_KSYMBOL_TYPE_MAX ||
	    ksym_type == PERF_RECORD_KSYMBOL_TYPE_UNKNOWN)
		goto err;

	strlcpy(name, sym, KSYM_NAME_LEN);
	name_len = strlen(name) + 1;
	while (!IS_ALIGNED(name_len, sizeof(u64)))
		name[name_len++] = '\0';
	BUILD_BUG_ON(KSYM_NAME_LEN % sizeof(u64));

	if (unregister)
		flags |= PERF_RECORD_KSYMBOL_FLAGS_UNREGISTER;

	ksymbol_event = (struct perf_ksymbol_event){
		.name = name,
		.name_len = name_len,
		.event_id = {
			.header = {
				.type = PERF_RECORD_KSYMBOL,
				.size = sizeof(ksymbol_event.event_id) +
					name_len,
			},
			.addr = addr,
			.len = len,
			.ksym_type = ksym_type,
			.flags = flags,
		},
	};

	perf_iterate_sb(perf_event_ksymbol_output, &ksymbol_event, NULL);
	return;
err:
	WARN_ONCE(1, "%s: Invalid KSYMBOL type 0x%x\n", __func__, ksym_type);
}