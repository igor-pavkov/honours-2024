static void lsr_read_preserve_aspect_ratio(GF_LASeRCodec *lsr, GF_Node *n)
{
	GF_FieldInfo info;
	u32 flag;
	SVG_PreserveAspectRatio *par;

	GF_LSR_READ_INT(lsr, flag, 1, "hasPreserveAspectRatio");
	if (!flag) return;

	lsr->last_error = gf_node_get_attribute_by_tag(n, TAG_SVG_ATT_preserveAspectRatio, 1, 0, &info);
	par = (SVG_PreserveAspectRatio *)info.far_ptr;

	GF_LSR_READ_INT(lsr, flag, 1, "choice (meetOrSlice)");
	GF_LSR_READ_INT(lsr, par->defer, 1, "choice (defer)");
	GF_LSR_READ_INT(lsr, flag, 4, "alignXandY");
	switch (flag) {
	case 1:
		par->align = SVG_PRESERVEASPECTRATIO_XMAXYMAX;
		break;
	case 2:
		par->align = SVG_PRESERVEASPECTRATIO_XMAXYMID;
		break;
	case 3:
		par->align = SVG_PRESERVEASPECTRATIO_XMAXYMIN;
		break;
	case 4:
		par->align = SVG_PRESERVEASPECTRATIO_XMIDYMAX;
		break;
	case 5:
		par->align = SVG_PRESERVEASPECTRATIO_XMIDYMID;
		break;
	case 6:
		par->align = SVG_PRESERVEASPECTRATIO_XMIDYMIN;
		break;
	case 7:
		par->align = SVG_PRESERVEASPECTRATIO_XMINYMAX;
		break;
	case 8:
		par->align = SVG_PRESERVEASPECTRATIO_XMINYMID;
		break;
	case 9:
		par->align = SVG_PRESERVEASPECTRATIO_XMINYMIN;
		break;
	default:
		par->align = SVG_PRESERVEASPECTRATIO_NONE;
		break;
	}
}