static bool l2cap_valid_mtu(struct l2cap_chan *chan, u16 mtu)
{
	switch (chan->scid) {
	case L2CAP_CID_LE_DATA:
		if (mtu < L2CAP_LE_MIN_MTU)
			return false;
		break;

	default:
		if (mtu < L2CAP_DEFAULT_MIN_MTU)
			return false;
	}

	return true;
}