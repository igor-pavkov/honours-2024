void perf_prepare_sample(struct perf_event_header *header,
			 struct perf_sample_data *data,
			 struct perf_event *event,
			 struct pt_regs *regs)
{
	u64 sample_type = event->attr.sample_type;

	header->type = PERF_RECORD_SAMPLE;
	header->size = sizeof(*header) + event->header_size;

	header->misc = 0;
	header->misc |= perf_misc_flags(regs);

	__perf_event_header__init_id(header, data, event);

	if (sample_type & PERF_SAMPLE_IP)
		data->ip = perf_instruction_pointer(regs);

	if (sample_type & PERF_SAMPLE_CALLCHAIN) {
		int size = 1;

		if (!(sample_type & __PERF_SAMPLE_CALLCHAIN_EARLY))
			data->callchain = perf_callchain(event, regs);

		size += data->callchain->nr;

		header->size += size * sizeof(u64);
	}

	if (sample_type & PERF_SAMPLE_RAW) {
		struct perf_raw_record *raw = data->raw;
		int size;

		if (raw) {
			struct perf_raw_frag *frag = &raw->frag;
			u32 sum = 0;

			do {
				sum += frag->size;
				if (perf_raw_frag_last(frag))
					break;
				frag = frag->next;
			} while (1);

			size = round_up(sum + sizeof(u32), sizeof(u64));
			raw->size = size - sizeof(u32);
			frag->pad = raw->size - sum;
		} else {
			size = sizeof(u64);
		}

		header->size += size;
	}

	if (sample_type & PERF_SAMPLE_BRANCH_STACK) {
		int size = sizeof(u64); /* nr */
		if (data->br_stack) {
			if (perf_sample_save_hw_index(event))
				size += sizeof(u64);

			size += data->br_stack->nr
			      * sizeof(struct perf_branch_entry);
		}
		header->size += size;
	}

	if (sample_type & (PERF_SAMPLE_REGS_USER | PERF_SAMPLE_STACK_USER))
		perf_sample_regs_user(&data->regs_user, regs,
				      &data->regs_user_copy);

	if (sample_type & PERF_SAMPLE_REGS_USER) {
		/* regs dump ABI info */
		int size = sizeof(u64);

		if (data->regs_user.regs) {
			u64 mask = event->attr.sample_regs_user;
			size += hweight64(mask) * sizeof(u64);
		}

		header->size += size;
	}

	if (sample_type & PERF_SAMPLE_STACK_USER) {
		/*
		 * Either we need PERF_SAMPLE_STACK_USER bit to be always
		 * processed as the last one or have additional check added
		 * in case new sample type is added, because we could eat
		 * up the rest of the sample size.
		 */
		u16 stack_size = event->attr.sample_stack_user;
		u16 size = sizeof(u64);

		stack_size = perf_sample_ustack_size(stack_size, header->size,
						     data->regs_user.regs);

		/*
		 * If there is something to dump, add space for the dump
		 * itself and for the field that tells the dynamic size,
		 * which is how many have been actually dumped.
		 */
		if (stack_size)
			size += sizeof(u64) + stack_size;

		data->stack_user_size = stack_size;
		header->size += size;
	}

	if (sample_type & PERF_SAMPLE_REGS_INTR) {
		/* regs dump ABI info */
		int size = sizeof(u64);

		perf_sample_regs_intr(&data->regs_intr, regs);

		if (data->regs_intr.regs) {
			u64 mask = event->attr.sample_regs_intr;

			size += hweight64(mask) * sizeof(u64);
		}

		header->size += size;
	}

	if (sample_type & PERF_SAMPLE_PHYS_ADDR)
		data->phys_addr = perf_virt_to_phys(data->addr);

#ifdef CONFIG_CGROUP_PERF
	if (sample_type & PERF_SAMPLE_CGROUP) {
		struct cgroup *cgrp;

		/* protected by RCU */
		cgrp = task_css_check(current, perf_event_cgrp_id, 1)->cgroup;
		data->cgroup = cgroup_id(cgrp);
	}
#endif

	if (sample_type & PERF_SAMPLE_AUX) {
		u64 size;

		header->size += sizeof(u64); /* size */

		/*
		 * Given the 16bit nature of header::size, an AUX sample can
		 * easily overflow it, what with all the preceding sample bits.
		 * Make sure this doesn't happen by using up to U16_MAX bytes
		 * per sample in total (rounded down to 8 byte boundary).
		 */
		size = min_t(size_t, U16_MAX - header->size,
			     event->attr.aux_sample_size);
		size = rounddown(size, 8);
		size = perf_prepare_sample_aux(event, data, size);

		WARN_ON_ONCE(size + header->size > U16_MAX);
		header->size += size;
	}
	/*
	 * If you're adding more sample types here, you likely need to do
	 * something about the overflowing header::size, like repurpose the
	 * lowest 3 bits of size, which should be always zero at the moment.
	 * This raises a more important question, do we really need 512k sized
	 * samples and why, so good argumentation is in order for whatever you
	 * do here next.
	 */
	WARN_ON_ONCE(header->size & 7);
}