void vhdx_guid_generate(MSGUID *guid)
{
    uuid_t uuid;
    assert(guid != NULL);

    uuid_generate(uuid);
    memcpy(guid, uuid, sizeof(MSGUID));
}