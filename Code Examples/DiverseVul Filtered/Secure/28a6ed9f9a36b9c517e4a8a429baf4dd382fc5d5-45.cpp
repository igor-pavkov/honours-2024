PHP_MINIT_FUNCTION(spl_dllist) /* {{{ */
{
	REGISTER_SPL_STD_CLASS_EX(SplDoublyLinkedList, spl_dllist_object_new, spl_funcs_SplDoublyLinkedList);
	memcpy(&spl_handler_SplDoublyLinkedList, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

	spl_handler_SplDoublyLinkedList.offset = XtOffsetOf(spl_dllist_object, std);
	spl_handler_SplDoublyLinkedList.clone_obj = spl_dllist_object_clone;
	spl_handler_SplDoublyLinkedList.count_elements = spl_dllist_object_count_elements;
	spl_handler_SplDoublyLinkedList.get_debug_info = spl_dllist_object_get_debug_info;
	spl_handler_SplDoublyLinkedList.get_gc = spl_dllist_object_get_gc;
	spl_handler_SplDoublyLinkedList.dtor_obj = zend_objects_destroy_object;
	spl_handler_SplDoublyLinkedList.free_obj = spl_dllist_object_free_storage;

	REGISTER_SPL_CLASS_CONST_LONG(SplDoublyLinkedList, "IT_MODE_LIFO",  SPL_DLLIST_IT_LIFO);
	REGISTER_SPL_CLASS_CONST_LONG(SplDoublyLinkedList, "IT_MODE_FIFO",  0);
	REGISTER_SPL_CLASS_CONST_LONG(SplDoublyLinkedList, "IT_MODE_DELETE",SPL_DLLIST_IT_DELETE);
	REGISTER_SPL_CLASS_CONST_LONG(SplDoublyLinkedList, "IT_MODE_KEEP",  0);

	REGISTER_SPL_IMPLEMENTS(SplDoublyLinkedList, Iterator);
	REGISTER_SPL_IMPLEMENTS(SplDoublyLinkedList, Countable);
	REGISTER_SPL_IMPLEMENTS(SplDoublyLinkedList, ArrayAccess);
	REGISTER_SPL_IMPLEMENTS(SplDoublyLinkedList, Serializable);

	spl_ce_SplDoublyLinkedList->get_iterator = spl_dllist_get_iterator;

	REGISTER_SPL_SUB_CLASS_EX(SplQueue,           SplDoublyLinkedList,        spl_dllist_object_new, spl_funcs_SplQueue);
	REGISTER_SPL_SUB_CLASS_EX(SplStack,           SplDoublyLinkedList,        spl_dllist_object_new, NULL);

	spl_ce_SplQueue->get_iterator = spl_dllist_get_iterator;
	spl_ce_SplStack->get_iterator = spl_dllist_get_iterator;

	return SUCCESS;
}