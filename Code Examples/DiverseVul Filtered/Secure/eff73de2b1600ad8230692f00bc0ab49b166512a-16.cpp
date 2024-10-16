static void add_APPn(struct camera_data *cam)
{
	if(cam->APP_len > 0) {
		cam->workbuff->data[cam->workbuff->length++] = 0xFF;
		cam->workbuff->data[cam->workbuff->length++] = 0xE0+cam->APPn;
		cam->workbuff->data[cam->workbuff->length++] = 0;
		cam->workbuff->data[cam->workbuff->length++] = cam->APP_len+2;
		memcpy(cam->workbuff->data+cam->workbuff->length,
		       cam->APP_data, cam->APP_len);
		cam->workbuff->length += cam->APP_len;
	}
}