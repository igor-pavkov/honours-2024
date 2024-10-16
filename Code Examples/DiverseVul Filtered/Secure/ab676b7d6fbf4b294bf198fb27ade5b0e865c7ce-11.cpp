static inline pagemap_entry_t make_pme(u64 val)
{
	return (pagemap_entry_t) { .pme = val };
}