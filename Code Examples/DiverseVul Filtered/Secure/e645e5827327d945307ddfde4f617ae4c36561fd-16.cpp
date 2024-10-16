static void free_object(pyc_object *object) {
	if (!object) {
		return;
	}
	switch (object->type) {
	case TYPE_SMALL_TUPLE:
	case TYPE_TUPLE:
		rz_list_free(object->data);
		break;
	case TYPE_STRING:
	case TYPE_TRUE:
	case TYPE_FALSE:
	case TYPE_INT:
	case TYPE_NONE:
	case TYPE_NULL:
	case TYPE_ASCII_INTERNED:
	case TYPE_SHORT_ASCII:
	case TYPE_ASCII:
	case TYPE_SHORT_ASCII_INTERNED:
		free(object->data);
		break;
	case TYPE_CODE_v0:
	case TYPE_CODE_v1: {
		pyc_code_object *cobj = object->data;
		free_object(cobj->code);
		free_object(cobj->consts);
		free_object(cobj->names);
		free_object(cobj->varnames);
		free_object(cobj->freevars);
		free_object(cobj->cellvars);
		free_object(cobj->filename);
		free_object(cobj->name);
		free_object(cobj->lnotab);
		free(object->data);
	} break;
	case TYPE_REF:
		free_object(object->data);
		break;
	case TYPE_SET:
	case TYPE_FROZENSET:
	case TYPE_ELLIPSIS:
	case TYPE_STOPITER:
	case TYPE_BINARY_COMPLEX:
	case TYPE_BINARY_FLOAT:
	case TYPE_COMPLEX:
	case TYPE_STRINGREF:
	case TYPE_DICT:
	case TYPE_FLOAT:
	case TYPE_INT64:
	case TYPE_INTERNED:
	case TYPE_LIST:
	case TYPE_LONG:
	case TYPE_UNICODE:
	case TYPE_UNKNOWN:
		RZ_LOG_ERROR("Free not implemented for type %x\n", object->type);
		break;
	default:
		RZ_LOG_ERROR("Undefined type in free_object (%x)\n", object->type);
		break;
	}
	free(object);
}