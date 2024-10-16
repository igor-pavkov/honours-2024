static int zr364xx_start_acquire(struct zr364xx_camera *cam)
{
	int j;

	DBG("start acquire\n");

	cam->last_frame = -1;
	cam->cur_frame = 0;
	for (j = 0; j < FRAMES; j++) {
		cam->buffer.frame[j].ulState = ZR364XX_READ_IDLE;
		cam->buffer.frame[j].cur_size = 0;
	}
	cam->b_acquire = 1;
	return 0;
}