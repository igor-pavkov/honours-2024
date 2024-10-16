const char *drill_m_code_name(drill_m_code_t m_code)
{
    switch (m_code) {
    case DRILL_M_END:
	return N_("end of program");
    case DRILL_M_PATTERNEND:
	return N_("pattern end");
    case DRILL_M_REPEATPATTERNOFFSET:
	return N_("repeat pattern offset");
    case DRILL_M_STOPOPTIONAL:
	return N_("stop optional");
    case DRILL_M_SANDREND:
	return N_("step and repeat end");
    case DRILL_M_STOPINSPECTION:
	return N_("stop for inspection");
    case DRILL_M_ZAXISROUTEPOSITIONDEPTHCTRL:
	return N_("Z-axis rout position with depth control");
    case DRILL_M_ZAXISROUTEPOSITION:
	return N_("Z-axis rout position");
    case DRILL_M_RETRACTCLAMPING:
	return N_("retract with clamping");
    case DRILL_M_RETRACTNOCLAMPING:
	return N_("retract without clamping");
    case DRILL_M_TOOLTIPCHECK:
	return N_("tool tip check");
    case DRILL_M_PATTERN:
	return N_("pattern start");
    case DRILL_M_ENDREWIND:
	return N_("end of program with rewind");
    case DRILL_M_MESSAGELONG:
	return N_("long operator message");
    case DRILL_M_MESSAGE:
	return N_("operator message");
    case DRILL_M_HEADER:
	return N_("header start");
    case DRILL_M_VISANDRPATTERN:
	return N_("vision step and repeat pattern start");
    case DRILL_M_VISANDRPATTERNREWIND:
	return N_("vision step and repeat rewind");
    case DRILL_M_VISANDRPATTERNOFFSETCOUNTERCTRL:
	return N_("vision step and repeat offset counter control");
    case DRILL_M_REFSCALING:
	return N_("reference scaling on");
    case DRILL_M_REFSCALINGEND:
	return N_("reference scaling off");
    case DRILL_M_PECKDRILLING:
	return N_("peck drilling on");
    case DRILL_M_PECKDRILLINGEND:
	return N_("peck drilling off");
    case DRILL_M_SWAPAXIS:
	return N_("swap axes");
    case DRILL_M_METRIC:
	return N_("metric measuring mode");
    case DRILL_M_IMPERIAL:
	return N_("inch measuring mode");
    case DRILL_M_MIRRORX:
	return N_("mirror image X-axis");
    case DRILL_M_MIRRORY:
	return N_("mirror image Y-axis");
    case DRILL_M_HEADEREND:
	return N_("header end");
    case DRILL_M_CANNEDTEXTX:
	return N_("canned text along X-axis");
    case DRILL_M_CANNEDTEXTY:
	return N_("canned text along Y-axis");
    case DRILL_M_USERDEFPATTERN:
	return N_("user defined stored pattern");

    case DRILL_M_UNKNOWN:
    default:
	return N_("unknown M-code");
    }
} /* drill_m_code_name() */