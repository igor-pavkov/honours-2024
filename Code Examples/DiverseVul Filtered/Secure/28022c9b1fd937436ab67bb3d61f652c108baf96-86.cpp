PHP_FUNCTION(imageaffine)
{
	zval *IM;
	gdImagePtr src;
	gdImagePtr dst;
	gdRect rect;
	gdRectPtr pRect = NULL;
	zval *z_rect = NULL;
	zval *z_affine;
	zval *tmp;
	double affine[6];
	int i, nelems;
	zval *zval_affine_elem = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ra|a", &IM, &z_affine, &z_rect) == FAILURE)  {
		return;
	}

	if ((src = (gdImagePtr)zend_fetch_resource(Z_RES_P(IM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	if ((nelems = zend_hash_num_elements(Z_ARRVAL_P(z_affine))) != 6) {
		php_error_docref(NULL, E_WARNING, "Affine array must have six elements");
		RETURN_FALSE;
	}

	for (i = 0; i < nelems; i++) {
		if ((zval_affine_elem = zend_hash_index_find(Z_ARRVAL_P(z_affine), i)) != NULL) {
			switch (Z_TYPE_P(zval_affine_elem)) {
				case IS_LONG:
					affine[i]  = Z_LVAL_P(zval_affine_elem);
					break;
				case IS_DOUBLE:
					affine[i] = Z_DVAL_P(zval_affine_elem);
					break;
				case IS_STRING:
					affine[i] = zval_get_double(zval_affine_elem);
					break;
				default:
					php_error_docref(NULL, E_WARNING, "Invalid type for element %i", i);
					RETURN_FALSE;
			}
		}
	}

	if (z_rect != NULL) {
		if ((tmp = zend_hash_str_find(Z_ARRVAL_P(z_rect), "x", sizeof("x") - 1)) != NULL) {
			rect.x = zval_get_long(tmp);
		} else {
			php_error_docref(NULL, E_WARNING, "Missing x position");
			RETURN_FALSE;
		}

		if ((tmp = zend_hash_str_find(Z_ARRVAL_P(z_rect), "y", sizeof("y") - 1)) != NULL) {
			rect.y = zval_get_long(tmp);
		} else {
			php_error_docref(NULL, E_WARNING, "Missing y position");
			RETURN_FALSE;
		}

		if ((tmp = zend_hash_str_find(Z_ARRVAL_P(z_rect), "width", sizeof("width") - 1)) != NULL) {
			rect.width = zval_get_long(tmp);
		} else {
			php_error_docref(NULL, E_WARNING, "Missing width");
			RETURN_FALSE;
		}

		if ((tmp = zend_hash_str_find(Z_ARRVAL_P(z_rect), "height", sizeof("height") - 1)) != NULL) {
			rect.height = zval_get_long(tmp);
		} else {
			php_error_docref(NULL, E_WARNING, "Missing height");
			RETURN_FALSE;
		}
		pRect = &rect;
	} else {
		rect.x = -1;
		rect.y = -1;
		rect.width = gdImageSX(src);
		rect.height = gdImageSY(src);
		pRect = NULL;
	}

	if (gdTransformAffineGetImage(&dst, src, pRect, affine) != GD_TRUE) {
		RETURN_FALSE;
	}

	if (dst == NULL) {
		RETURN_FALSE;
	} else {
		RETURN_RES(zend_register_resource(dst, le_gd));
	}
}