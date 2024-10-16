int SN_Client_Disconnect(MqttClient *client)
{
    return SN_Client_Disconnect_ex(client, NULL);
}