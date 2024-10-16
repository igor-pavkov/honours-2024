static void php_iconv_stream_filter_cleanup(php_stream_filter *filter)
{
	php_iconv_stream_filter_dtor((php_iconv_stream_filter *)Z_PTR(filter->abstract));
	pefree(Z_PTR(filter->abstract), ((php_iconv_stream_filter *)Z_PTR(filter->abstract))->persistent);
}