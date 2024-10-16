static int ext4_get_next_id(struct super_block *sb, struct kqid *qid)
{
	const struct quota_format_ops	*ops;

	if (!sb_has_quota_loaded(sb, qid->type))
		return -ESRCH;
	ops = sb_dqopt(sb)->ops[qid->type];
	if (!ops || !ops->get_next_id)
		return -ENOSYS;
	return dquot_get_next_id(sb, qid);
}