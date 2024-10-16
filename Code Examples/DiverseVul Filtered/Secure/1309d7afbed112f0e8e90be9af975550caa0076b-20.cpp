static struct tpm_chip *tpm_chip_find_get(int chip_num)
{
	struct tpm_chip *pos, *chip = NULL;

	rcu_read_lock();
	list_for_each_entry_rcu(pos, &tpm_chip_list, list) {
		if (chip_num != TPM_ANY_NUM && chip_num != pos->dev_num)
			continue;

		if (try_module_get(pos->dev->driver->owner)) {
			chip = pos;
			break;
		}
	}
	rcu_read_unlock();
	return chip;
}