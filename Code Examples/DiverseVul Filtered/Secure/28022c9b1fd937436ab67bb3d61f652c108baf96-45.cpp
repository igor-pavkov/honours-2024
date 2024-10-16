static gdFontPtr php_find_gd_font(int size)
{
	gdFontPtr font;

	switch (size) {
		case 1:
			font = gdFontTiny;
			break;
		case 2:
			font = gdFontSmall;
			break;
		case 3:
			font = gdFontMediumBold;
			break;
		case 4:
			font = gdFontLarge;
			break;
		case 5:
			font = gdFontGiant;
			break;
		default: {
			 zval *zv = zend_hash_index_find(&EG(regular_list), size - 5);
			 if (!zv || (Z_RES_P(zv))->type != le_gd_font) {
				 if (size < 1) {
					 font = gdFontTiny;
				 } else {
					 font = gdFontGiant;
				 }
			 } else {
				 font = (gdFontPtr)Z_RES_P(zv)->ptr;
			 }
		 }
		 break;
	}

	return font;
}