static int vfswrap_asys_state_destructor(struct vfswrap_asys_state *s)
{
	asys_cancel(s->asys_ctx, s->req);
	return 0;
}