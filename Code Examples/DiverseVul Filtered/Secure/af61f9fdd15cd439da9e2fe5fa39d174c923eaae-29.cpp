bool looks_like_pbl(const std::string& file)
{
	return utils::wildcard_string_match(utils::lowercase(file), "*.pbl");
}