void print_report_hdr(uint64_t flags, struct tm *rectime,
		      struct file_header *file_hdr)
{

	/* Get date of file creation */
	get_file_timestamp_struct(flags, rectime, file_hdr);

	/*
	 * Display the header.
	 * NB: Number of CPU (value in [1, NR_CPUS + 1]).
	 * 	1 means that there is only one proc and non SMP kernel.
	 *	2 means one proc and SMP kernel. Etc.
	 */
	print_gal_header(rectime, file_hdr->sa_sysname, file_hdr->sa_release,
			 file_hdr->sa_nodename, file_hdr->sa_machine,
			 file_hdr->sa_cpu_nr > 1 ? file_hdr->sa_cpu_nr - 1 : 1,
			 PLAIN_OUTPUT);
}