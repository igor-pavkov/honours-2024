int sftp_send_client_message(sftp_session sftp, sftp_client_message msg){
	return sftp_packet_write(sftp, msg->type, msg->complete_message);
}