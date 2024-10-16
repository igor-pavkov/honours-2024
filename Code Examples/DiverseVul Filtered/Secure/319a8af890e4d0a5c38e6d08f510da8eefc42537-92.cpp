static double screen_units(double d)
{
	switch (screen.unit) {
	case GERBV_INS:
		return COORD2INS(d);
		break;
	case GERBV_MILS:
		return COORD2MILS(d);
		break;
	case GERBV_MMS:
		return COORD2MMS(d);
		break;
	}

	return d;
}