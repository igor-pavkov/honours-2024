PHP_FUNCTION(imagefilter)
{
	zval *tmp;

	typedef void (*image_filter)(INTERNAL_FUNCTION_PARAMETERS);
	zend_long filtertype;
	image_filter filters[] =
	{
		php_image_filter_negate ,
		php_image_filter_grayscale,
		php_image_filter_brightness,
		php_image_filter_contrast,
		php_image_filter_colorize,
		php_image_filter_edgedetect,
		php_image_filter_emboss,
		php_image_filter_gaussian_blur,
		php_image_filter_selective_blur,
		php_image_filter_mean_removal,
		php_image_filter_smooth,
		php_image_filter_pixelate
	};

	if (ZEND_NUM_ARGS() < 2 || ZEND_NUM_ARGS() > IMAGE_FILTER_MAX_ARGS) {
		WRONG_PARAM_COUNT;
	} else if (zend_parse_parameters(2, "rl", &tmp, &filtertype) == FAILURE) {
		return;
	}

	if (filtertype >= 0 && filtertype <= IMAGE_FILTER_MAX) {
		filters[filtertype](INTERNAL_FUNCTION_PARAM_PASSTHRU);
	}
}