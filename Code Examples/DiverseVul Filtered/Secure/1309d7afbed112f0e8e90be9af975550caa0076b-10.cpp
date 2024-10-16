static void user_reader_timeout(unsigned long ptr)
{
	struct tpm_chip *chip = (struct tpm_chip *) ptr;

	schedule_work(&chip->work);
}