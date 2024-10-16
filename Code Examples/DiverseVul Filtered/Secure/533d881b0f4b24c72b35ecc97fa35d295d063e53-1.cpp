void sftp_client_message_set_filename(sftp_client_message msg, const char *newname){
	free(msg->filename);
	msg->filename = strdup(newname);
}