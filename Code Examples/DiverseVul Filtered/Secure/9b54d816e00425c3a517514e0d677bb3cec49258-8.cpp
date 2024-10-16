int blkg_print_stat_ios_recursive(struct seq_file *sf, void *v)
{
	blkcg_print_blkgs(sf, css_to_blkcg(seq_css(sf)),
			  blkg_prfill_rwstat_field_recursive,
			  (void *)seq_cft(sf)->private,
			  offsetof(struct blkcg_gq, stat_ios), true);
	return 0;
}