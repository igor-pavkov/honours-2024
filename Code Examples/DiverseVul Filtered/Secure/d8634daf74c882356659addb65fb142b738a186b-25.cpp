void DetectSignatureApplyActions(Packet *p,
        const Signature *s, const uint8_t alert_flags)
{
    PACKET_UPDATE_ACTION(p, s->action);

    if (s->action & ACTION_DROP) {
        if (p->alerts.drop.action == 0) {
            p->alerts.drop.num = s->num;
            p->alerts.drop.action = s->action;
            p->alerts.drop.s = (Signature *)s;
        }
    } else if (s->action & ACTION_PASS) {
        /* if an stream/app-layer match we enforce the pass for the flow */
        if ((p->flow != NULL) &&
                (alert_flags & (PACKET_ALERT_FLAG_STATE_MATCH|PACKET_ALERT_FLAG_STREAM_MATCH)))
        {
            FlowSetNoPacketInspectionFlag(p->flow);
        }

    }
}