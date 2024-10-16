const char *sftp_client_message_get_data(sftp_client_message msg){
	if (msg->str_data == NULL)
		msg->str_data = ssh_string_to_char(msg->data);
	return msg->str_data;
}