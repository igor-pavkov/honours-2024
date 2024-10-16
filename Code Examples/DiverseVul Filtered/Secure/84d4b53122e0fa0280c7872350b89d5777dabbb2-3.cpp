int SN_Client_WaitMessage_ex(MqttClient *client, SN_Object* packet_obj,
        int timeout_ms)
{
    return SN_Client_WaitType(client, packet_obj,
        SN_MSG_TYPE_ANY, 0, timeout_ms);
}