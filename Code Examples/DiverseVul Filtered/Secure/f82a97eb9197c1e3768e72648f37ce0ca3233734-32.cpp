const char *server_feature_value(const char *feature, int *len)
{
	return parse_feature_value(server_capabilities, feature, len);
}