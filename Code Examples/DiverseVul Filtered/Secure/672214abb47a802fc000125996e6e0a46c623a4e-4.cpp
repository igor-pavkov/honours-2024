const char *drill_g_code_name(drill_g_code_t g_code)
{
    switch (g_code) {
    case DRILL_G_ROUT:
	return N_("rout mode");
    case DRILL_G_LINEARMOVE:
	return N_("linear mode");
    case DRILL_G_CWMOVE:
	return N_("circular CW mode");
    case DRILL_G_CCWMOVE:
	return N_("circular CCW mode");
    case DRILL_G_VARIABLEDWELL:
	return N_("variable dwell");
    case DRILL_G_DRILL:
	return N_("drill mode");
    case DRILL_G_OVERRIDETOOLSPEED:
	return N_("override tool feed or speed");
    case DRILL_G_ROUTCIRCLE:
	return N_("routed CW circle");
    case DRILL_G_ROUTCIRCLECCW:
	return N_("routed CCW circle");
    case DRILL_G_VISTOOL:
	return N_("select vision tool");
    case DRILL_G_VISSINGLEPOINTOFFSET:
	return N_("single point vision offset");
    case DRILL_G_VISMULTIPOINTTRANS:
	return N_("multipoint vision translation");
    case DRILL_G_VISCANCEL:
	return N_("cancel vision translation or offset");
    case DRILL_G_VISCORRHOLEDRILL:
	return N_("vision corrected single hole drilling");
    case DRILL_G_VISAUTOCALIBRATION:
	return N_("vision system autocalibration");
    case DRILL_G_CUTTERCOMPOFF:
	return N_("cutter compensation off");
    case DRILL_G_CUTTERCOMPLEFT:
	return N_("cutter compensation left");
    case DRILL_G_CUTTERCOMPRIGHT:
	return N_("cutter compensation right");
    case DRILL_G_VISSINGLEPOINTOFFSETREL:
	return N_("single point vision relative offset");
    case DRILL_G_VISMULTIPOINTTRANSREL:
	return N_("multipoint vision relative translation");
    case DRILL_G_VISCANCELREL:
	return N_("cancel vision relative translation or offset");
    case DRILL_G_VISCORRHOLEDRILLREL:
	return N_("vision corrected single hole relative drilling");
    case DRILL_G_PACKDIP2:
	return N_("dual in line package");
    case DRILL_G_PACKDIP:
	return N_("dual in line package");
    case DRILL_G_PACK8PINL:
	return N_("eight pin L package");
    case DRILL_G_CIRLE:
	return N_("canned circle");
    case DRILL_G_SLOT:
	return N_("canned slot");
    case DRILL_G_ROUTSLOT:
	return N_("routed step slot");
    case DRILL_G_ABSOLUTE:
	return N_("absolute input mode");
    case DRILL_G_INCREMENTAL:
	return N_("incremental input mode");
    case DRILL_G_ZEROSET:
	return N_("zero set");

    case DRILL_G_UNKNOWN:
    default:
	return N_("unknown G-code");
    }
} /* drill_g_code_name() */