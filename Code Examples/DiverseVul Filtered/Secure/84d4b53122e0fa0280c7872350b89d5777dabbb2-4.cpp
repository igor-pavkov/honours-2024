int MqttClient_PropsFree(MqttProp *head)
{
    return MqttProps_Free(head);
}