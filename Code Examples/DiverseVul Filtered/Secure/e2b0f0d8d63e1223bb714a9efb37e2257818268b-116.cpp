static struct avrcp_player *target_get_player(struct avrcp *session)
{
	if (!session->target)
		return NULL;

	return session->target->player;
}