SPL_METHOD(SplFileInfo, _bad_state_ex)
{
	zend_throw_exception_ex(spl_ce_LogicException, 0,
		"The parent constructor was not called: the object is in an "
		"invalid state ");
}