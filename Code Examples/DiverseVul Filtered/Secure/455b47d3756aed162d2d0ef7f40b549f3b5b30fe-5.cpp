int del_session(ctrl_t *ctrl, int isftp)
{
	DBG("%sFTP Client session ended.", isftp ? "": "T" );

	if (!ctrl)
		return -1;

	if (isftp && ctrl->sd > 0) {
		shutdown(ctrl->sd, SHUT_RDWR);
		close(ctrl->sd);
	}

	if (ctrl->data_listen_sd > 0) {
		shutdown(ctrl->data_listen_sd, SHUT_RDWR);
		close(ctrl->data_listen_sd);
	}

	if (ctrl->data_sd > 0) {
		shutdown(ctrl->data_sd, SHUT_RDWR);
		close(ctrl->data_sd);
	}

	if (ctrl->buf)
		free(ctrl->buf);

	if (!inetd && ctrl->ctx)
		free(ctrl->ctx);
	free(ctrl);

	return 0;
}