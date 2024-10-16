static point cvtPt(pointf p, int rankdir)
{
    pointf q = { 0, 0 };
    point Q;

    switch (rankdir) {
    case RANKDIR_TB:
	q = p;
	break;
    case RANKDIR_BT:
	q.x = p.x;
	q.y = -p.y;
	break;
    case RANKDIR_LR:
	q.y = p.x;
	q.x = -p.y;
	break;
    case RANKDIR_RL:
	q.y = p.x;
	q.x = p.y;
	break;
    }
    PF2P(q, Q);
    return Q;
}