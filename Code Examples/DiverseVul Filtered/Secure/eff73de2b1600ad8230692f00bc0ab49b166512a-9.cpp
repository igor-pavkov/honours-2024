static void add_COM(struct camera_data *cam)
{
	if(cam->COM_len > 0) {
		cam->workbuff->data[cam->workbuff->length++] = 0xFF;
		cam->workbuff->data[cam->workbuff->length++] = 0xFE;
		cam->workbuff->data[cam->workbuff->length++] = 0;
		cam->workbuff->data[cam->workbuff->length++] = cam->COM_len+2;
		memcpy(cam->workbuff->data+cam->workbuff->length,
		       cam->COM_data, cam->COM_len);
		cam->workbuff->length += cam->COM_len;
	}
}