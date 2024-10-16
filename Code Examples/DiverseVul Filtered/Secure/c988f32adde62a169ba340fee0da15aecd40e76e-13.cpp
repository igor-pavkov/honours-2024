void ssl_set_curves( ssl_context *ssl, const ecp_group_id *curve_list )
{
  ssl->curve_list = curve_list;
}