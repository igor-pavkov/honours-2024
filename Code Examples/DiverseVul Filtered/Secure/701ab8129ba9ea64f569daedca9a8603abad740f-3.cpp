void imcb_file_canceled(struct im_connection *ic, file_transfer_t *file, char *reason)
{
	bee_t *bee = ic->bee;

	if (file->canceled) {
		file->canceled(file, reason);
	}

	if (bee->ui->ft_close) {
		bee->ui->ft_close(ic, file);
	}
}