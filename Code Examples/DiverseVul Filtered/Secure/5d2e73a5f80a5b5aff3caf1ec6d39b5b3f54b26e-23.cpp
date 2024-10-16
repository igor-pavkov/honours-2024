static inline int zr364xx_stop_acquire(struct zr364xx_camera *cam)
{
	cam->b_acquire = 0;
	return 0;
}