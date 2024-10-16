static double invflip_angle(double angle, int rankdir)
{
    switch (rankdir) {
    case RANKDIR_TB:
	break;
    case RANKDIR_BT:
	angle *= -1;
	break;
    case RANKDIR_LR:
	angle -= M_PI * 0.5;
	break;
    case RANKDIR_RL:
	if (angle == M_PI)
	    angle = -0.5 * M_PI;
	else if (angle == M_PI * 0.75)
	    angle = -0.25 * M_PI;
	else if (angle == M_PI * 0.5)
	    angle = 0;
/* clang complains about self assignment of double
	else if (angle == M_PI * 0.25)
	    angle = angle;
 */
	else if (angle == 0)
	    angle = M_PI * 0.5;
	else if (angle == M_PI * -0.25)
	    angle = M_PI * 0.75;
	else if (angle == M_PI * -0.5)
	    angle = M_PI;
/* clang complains about self assignment of double
	else if (angle == M_PI * -0.75)
	    angle = angle;
 */
	break;
    }
    return angle;
}