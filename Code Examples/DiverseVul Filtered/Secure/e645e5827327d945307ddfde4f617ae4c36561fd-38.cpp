static pyc_object *copy_object(pyc_object *object) {
	pyc_object *copy = RZ_NEW0(pyc_object);
	if (!copy || !object) {
		free(copy);
		return NULL;
	}
	copy->type = object->type;
	switch (object->type) {
	case TYPE_NULL:
		break;
	case TYPE_TUPLE:
	case TYPE_SMALL_TUPLE:
		copy->data = rz_list_clone(object->data);
		break;
	case TYPE_INT:
	case TYPE_INT64:
	case TYPE_NONE:
	case TYPE_TRUE:
	case TYPE_FALSE:
	case TYPE_STRING:
	case TYPE_ASCII:
	case TYPE_SHORT_ASCII:
	case TYPE_ASCII_INTERNED:
	case TYPE_SHORT_ASCII_INTERNED:
		copy->data = strdup(object->data);
		break;
	case TYPE_CODE_v0:
	case TYPE_CODE_v1: {
		pyc_code_object *src = object->data;
		pyc_code_object *dst = RZ_NEW0(pyc_code_object);
		if (!dst) {
			break;
		}
		memcpy(dst, src, sizeof(*dst));
		dst->code = copy_object(src->code);
		dst->consts = copy_object(src->consts);
		dst->names = copy_object(src->names);
		dst->varnames = copy_object(src->varnames);
		dst->freevars = copy_object(src->freevars);
		dst->cellvars = copy_object(src->cellvars);
		dst->filename = copy_object(src->filename);
		dst->name = copy_object(src->name);
		dst->lnotab = copy_object(src->lnotab);
		copy->data = dst;
	} break;
	case TYPE_REF:
		copy->data = copy_object(object->data);
		break;
	case TYPE_ELLIPSIS:
	case TYPE_STOPITER:
	case TYPE_BINARY_COMPLEX:
	case TYPE_BINARY_FLOAT:
	case TYPE_COMPLEX:
	case TYPE_STRINGREF:
	case TYPE_DICT:
	case TYPE_FLOAT:
	case TYPE_FROZENSET:
	case TYPE_INTERNED:
	case TYPE_LIST:
	case TYPE_LONG:
	case TYPE_SET:
	case TYPE_UNICODE:
	case TYPE_UNKNOWN:
		RZ_LOG_ERROR("Copy not implemented for type %x\n", object->type);
		break;
	default:
		RZ_LOG_ERROR("Undefined type in copy_object (%x)\n", object->type);
		break;
	}
	if (!copy->data) {
		RZ_FREE(copy);
	}
	return copy;
}