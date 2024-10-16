static BOOL update_bounds_equals(rdpBounds* bounds1, rdpBounds* bounds2)
{
	if ((bounds1->left == bounds2->left) && (bounds1->top == bounds2->top) &&
	    (bounds1->right == bounds2->right) && (bounds1->bottom == bounds2->bottom))
		return TRUE;

	return FALSE;
}