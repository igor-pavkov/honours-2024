void jbd2_journal_set_triggers(struct buffer_head *bh,
			       struct jbd2_buffer_trigger_type *type)
{
	struct journal_head *jh = jbd2_journal_grab_journal_head(bh);

	if (WARN_ON(!jh))
		return;
	jh->b_triggers = type;
	jbd2_journal_put_journal_head(jh);
}