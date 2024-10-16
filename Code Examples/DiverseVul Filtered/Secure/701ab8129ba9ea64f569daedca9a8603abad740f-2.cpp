void imcb_file_finished(struct im_connection *ic, file_transfer_t *file)
{
	bee_t *bee = ic->bee;

	if (bee->ui->ft_finished) {
		bee->ui->ft_finished(ic, file);
	}
}