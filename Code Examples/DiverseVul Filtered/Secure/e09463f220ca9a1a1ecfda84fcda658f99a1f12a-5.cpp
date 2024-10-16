void jbd2_buffer_abort_trigger(struct journal_head *jh,
			       struct jbd2_buffer_trigger_type *triggers)
{
	if (!triggers || !triggers->t_abort)
		return;

	triggers->t_abort(triggers, jh2bh(jh));
}