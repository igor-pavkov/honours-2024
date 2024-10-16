void cipso_v4_doi_free(struct cipso_v4_doi *doi_def)
{
	if (!doi_def)
		return;

	switch (doi_def->type) {
	case CIPSO_V4_MAP_TRANS:
		kfree(doi_def->map.std->lvl.cipso);
		kfree(doi_def->map.std->lvl.local);
		kfree(doi_def->map.std->cat.cipso);
		kfree(doi_def->map.std->cat.local);
		break;
	}
	kfree(doi_def);
}