static void ff_layout_sort_mirrors(struct nfs4_ff_layout_segment *fls)
{
	int i, j;

	for (i = 0; i < fls->mirror_array_cnt - 1; i++) {
		for (j = i + 1; j < fls->mirror_array_cnt; j++)
			if (fls->mirror_array[i]->efficiency <
			    fls->mirror_array[j]->efficiency)
				swap(fls->mirror_array[i],
				     fls->mirror_array[j]);
	}
}