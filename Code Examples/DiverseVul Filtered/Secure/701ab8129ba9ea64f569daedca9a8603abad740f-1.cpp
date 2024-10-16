gboolean imcb_file_recv_start(struct im_connection *ic, file_transfer_t *ft)
{
	bee_t *bee = ic->bee;

	if (bee->ui->ft_out_start) {
		return bee->ui->ft_out_start(ic, ft);
	} else {
		return FALSE;
	}
}