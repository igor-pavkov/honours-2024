bool CConnectionTransportUDP::SendPacket( const void *pkt, int cbPkt )
{
	iovec temp;
	temp.iov_base = const_cast<void*>( pkt );
	temp.iov_len = cbPkt;
	return SendPacketGather( 1, &temp, cbPkt );
}