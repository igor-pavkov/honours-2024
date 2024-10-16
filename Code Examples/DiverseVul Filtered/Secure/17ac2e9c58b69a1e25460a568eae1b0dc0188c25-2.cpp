unsigned int rose_new_lci(struct rose_neigh *neigh)
{
	int lci;

	if (neigh->dce_mode) {
		for (lci = 1; lci <= sysctl_rose_maximum_vcs; lci++)
			if (rose_find_socket(lci, neigh) == NULL && rose_route_free_lci(lci, neigh) == NULL)
				return lci;
	} else {
		for (lci = sysctl_rose_maximum_vcs; lci > 0; lci--)
			if (rose_find_socket(lci, neigh) == NULL && rose_route_free_lci(lci, neigh) == NULL)
				return lci;
	}

	return 0;
}