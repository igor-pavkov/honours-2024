static int next_target(struct dm_target_spec *last, uint32_t next, void *end,
		       struct dm_target_spec **spec, char **target_params)
{
	*spec = (struct dm_target_spec *) ((unsigned char *) last + next);
	*target_params = (char *) (*spec + 1);

	if (*spec < (last + 1))
		return -EINVAL;

	return invalid_str(*target_params, end);
}