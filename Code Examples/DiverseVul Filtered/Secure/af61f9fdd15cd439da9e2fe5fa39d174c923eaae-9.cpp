file_tree_checksum::file_tree_checksum(const config& cfg) :
	nfiles	(static_cast<size_t>(cfg["nfiles"].to_double())),
	sum_size(static_cast<size_t>(cfg["size"].to_double())),
	modified(static_cast<time_t>(cfg["modified"].to_double()))
{
}