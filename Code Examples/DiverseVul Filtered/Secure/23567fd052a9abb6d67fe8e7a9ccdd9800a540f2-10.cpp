bool lookup_user_key_possessed(const struct key *key,
			       const struct key_match_data *match_data)
{
	return key == match_data->raw_data;
}