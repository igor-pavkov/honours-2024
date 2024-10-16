PHP_FUNCTION(imageconvolution)
{
	zval *SIM, *hash_matrix;
	zval *var = NULL, *var2 = NULL;
	gdImagePtr im_src = NULL;
	double div, offset;
	int nelem, i, j, res;
	float matrix[3][3] = {{0,0,0}, {0,0,0}, {0,0,0}};

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "radd", &SIM, &hash_matrix, &div, &offset) == FAILURE) {
		RETURN_FALSE;
	}

	if ((im_src = (gdImagePtr)zend_fetch_resource(Z_RES_P(SIM), "Image", le_gd)) == NULL) {
		RETURN_FALSE;
	}

	nelem = zend_hash_num_elements(Z_ARRVAL_P(hash_matrix));
	if (nelem != 3) {
		php_error_docref(NULL, E_WARNING, "You must have 3x3 array");
		RETURN_FALSE;
	}

	for (i=0; i<3; i++) {
		if ((var = zend_hash_index_find(Z_ARRVAL_P(hash_matrix), (i))) != NULL && Z_TYPE_P(var) == IS_ARRAY) {
			if (zend_hash_num_elements(Z_ARRVAL_P(var)) != 3 ) {
				php_error_docref(NULL, E_WARNING, "You must have 3x3 array");
				RETURN_FALSE;
			}

			for (j=0; j<3; j++) {
				if ((var2 = zend_hash_index_find(Z_ARRVAL_P(var), j)) != NULL) {
					matrix[i][j] = (float) zval_get_double(var2);
				} else {
					php_error_docref(NULL, E_WARNING, "You must have a 3x3 matrix");
					RETURN_FALSE;
				}
			}
		}
	}
	res = gdImageConvolution(im_src, matrix, (float)div, (float)offset);

	if (res) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}