static int ftrace_check_record(struct dyn_ftrace *rec, int enable, int update)
{
	unsigned long flag = 0UL;

	/*
	 * If we are updating calls:
	 *
	 *   If the record has a ref count, then we need to enable it
	 *   because someone is using it.
	 *
	 *   Otherwise we make sure its disabled.
	 *
	 * If we are disabling calls, then disable all records that
	 * are enabled.
	 */
	if (enable && (rec->flags & ~FTRACE_FL_MASK))
		flag = FTRACE_FL_ENABLED;

	/*
	 * If enabling and the REGS flag does not match the REGS_EN, then
	 * do not ignore this record. Set flags to fail the compare against
	 * ENABLED.
	 */
	if (flag &&
	    (!(rec->flags & FTRACE_FL_REGS) != !(rec->flags & FTRACE_FL_REGS_EN)))
		flag |= FTRACE_FL_REGS;

	/* If the state of this record hasn't changed, then do nothing */
	if ((rec->flags & FTRACE_FL_ENABLED) == flag)
		return FTRACE_UPDATE_IGNORE;

	if (flag) {
		/* Save off if rec is being enabled (for return value) */
		flag ^= rec->flags & FTRACE_FL_ENABLED;

		if (update) {
			rec->flags |= FTRACE_FL_ENABLED;
			if (flag & FTRACE_FL_REGS) {
				if (rec->flags & FTRACE_FL_REGS)
					rec->flags |= FTRACE_FL_REGS_EN;
				else
					rec->flags &= ~FTRACE_FL_REGS_EN;
			}
		}

		/*
		 * If this record is being updated from a nop, then
		 *   return UPDATE_MAKE_CALL.
		 * Otherwise, if the EN flag is set, then return
		 *   UPDATE_MODIFY_CALL_REGS to tell the caller to convert
		 *   from the non-save regs, to a save regs function.
		 * Otherwise,
		 *   return UPDATE_MODIFY_CALL to tell the caller to convert
		 *   from the save regs, to a non-save regs function.
		 */
		if (flag & FTRACE_FL_ENABLED)
			return FTRACE_UPDATE_MAKE_CALL;
		else if (rec->flags & FTRACE_FL_REGS_EN)
			return FTRACE_UPDATE_MODIFY_CALL_REGS;
		else
			return FTRACE_UPDATE_MODIFY_CALL;
	}

	if (update) {
		/* If there's no more users, clear all flags */
		if (!(rec->flags & ~FTRACE_FL_MASK))
			rec->flags = 0;
		else
			/* Just disable the record (keep REGS state) */
			rec->flags &= ~FTRACE_FL_ENABLED;
	}

	return FTRACE_UPDATE_MAKE_NOP;
}