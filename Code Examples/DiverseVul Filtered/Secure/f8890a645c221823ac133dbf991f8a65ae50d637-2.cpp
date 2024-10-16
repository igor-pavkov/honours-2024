static BOOL update_bounds_is_null(rdpBounds* bounds)
{
	if ((bounds->left == 0) && (bounds->top == 0) && (bounds->right == 0) && (bounds->bottom == 0))
		return TRUE;

	return FALSE;
}