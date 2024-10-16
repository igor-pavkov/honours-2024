static BOOL update_read_ellipse_sc_order(wStream* s, const ORDER_INFO* orderInfo,
                                         ELLIPSE_SC_ORDER* ellipse_sc)
{
	ORDER_FIELD_COORD(1, ellipse_sc->leftRect);
	ORDER_FIELD_COORD(2, ellipse_sc->topRect);
	ORDER_FIELD_COORD(3, ellipse_sc->rightRect);
	ORDER_FIELD_COORD(4, ellipse_sc->bottomRect);
	ORDER_FIELD_BYTE(5, ellipse_sc->bRop2);
	ORDER_FIELD_BYTE(6, ellipse_sc->fillMode);
	ORDER_FIELD_COLOR(orderInfo, s, 7, &ellipse_sc->color);
	return TRUE;
}