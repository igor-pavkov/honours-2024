struct aq_stats_s *hw_atl_utils_get_hw_stats(struct aq_hw_s *self)
{
	return &self->curr_stats;
}