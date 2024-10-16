static struct ccid_operations *ccid_by_number(const u8 id)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(ccids); i++)
		if (ccids[i]->ccid_id == id)
			return ccids[i];
	return NULL;
}