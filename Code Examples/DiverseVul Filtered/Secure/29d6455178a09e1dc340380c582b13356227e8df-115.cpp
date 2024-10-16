static inline int preempt_count_equals(int preempt_offset)
{
	int nested = preempt_count() + rcu_preempt_depth();

	return (nested == preempt_offset);
}