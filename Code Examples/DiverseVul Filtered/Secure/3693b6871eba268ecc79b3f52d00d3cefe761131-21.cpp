void Server::process_PlayerPos(RemotePlayer *player, PlayerSAO *playersao,
	NetworkPacket *pkt)
{
	if (pkt->getRemainingBytes() < 12 + 12 + 4 + 4 + 4 + 1 + 1)
		return;

	v3s32 ps, ss;
	s32 f32pitch, f32yaw;
	u8 f32fov;

	*pkt >> ps;
	*pkt >> ss;
	*pkt >> f32pitch;
	*pkt >> f32yaw;

	f32 pitch = (f32)f32pitch / 100.0f;
	f32 yaw = (f32)f32yaw / 100.0f;
	u32 keyPressed = 0;

	// default behavior (in case an old client doesn't send these)
	f32 fov = 0;
	u8 wanted_range = 0;

	*pkt >> keyPressed;
	*pkt >> f32fov;
	fov = (f32)f32fov / 80.0f;
	*pkt >> wanted_range;

	v3f position((f32)ps.X / 100.0f, (f32)ps.Y / 100.0f, (f32)ps.Z / 100.0f);
	v3f speed((f32)ss.X / 100.0f, (f32)ss.Y / 100.0f, (f32)ss.Z / 100.0f);

	pitch = modulo360f(pitch);
	yaw = wrapDegrees_0_360(yaw);

	playersao->setBasePosition(position);
	player->setSpeed(speed);
	playersao->setLookPitch(pitch);
	playersao->setPlayerYaw(yaw);
	playersao->setFov(fov);
	playersao->setWantedRange(wanted_range);

	player->keyPressed = keyPressed;
	player->control.up    = (keyPressed & (0x1 << 0));
	player->control.down  = (keyPressed & (0x1 << 1));
	player->control.left  = (keyPressed & (0x1 << 2));
	player->control.right = (keyPressed & (0x1 << 3));
	player->control.jump  = (keyPressed & (0x1 << 4));
	player->control.aux1  = (keyPressed & (0x1 << 5));
	player->control.sneak = (keyPressed & (0x1 << 6));
	player->control.dig   = (keyPressed & (0x1 << 7));
	player->control.place = (keyPressed & (0x1 << 8));
	player->control.zoom  = (keyPressed & (0x1 << 9));

	if (playersao->checkMovementCheat()) {
		// Call callbacks
		m_script->on_cheat(playersao, "moved_too_fast");
		SendMovePlayer(pkt->getPeerId());
	}
}