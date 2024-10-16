static int invflip_side(int side, int rankdir)
{
    switch (rankdir) {
    case RANKDIR_TB:
	break;
    case RANKDIR_BT:
	switch (side) {
	case TOP:
	    side = BOTTOM;
	    break;
	case BOTTOM:
	    side = TOP;
	    break;
	default:
	    break;
	}
	break;
    case RANKDIR_LR:
	switch (side) {
	case TOP:
	    side = RIGHT;
	    break;
	case BOTTOM:
	    side = LEFT;
	    break;
	case LEFT:
	    side = TOP;
	    break;
	case RIGHT:
	    side = BOTTOM;
	    break;
	}
	break;
    case RANKDIR_RL:
	switch (side) {
	case TOP:
	    side = RIGHT;
	    break;
	case BOTTOM:
	    side = LEFT;
	    break;
	case LEFT:
	    side = BOTTOM;
	    break;
	case RIGHT:
	    side = TOP;
	    break;
	}
	break;
    }
    return side;
}