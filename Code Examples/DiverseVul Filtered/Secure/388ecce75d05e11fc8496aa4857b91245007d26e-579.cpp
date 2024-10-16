GF_Box *uuid_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_UnknownUUIDBox, GF_ISOM_BOX_TYPE_UUID);
	return (GF_Box *) tmp;
}