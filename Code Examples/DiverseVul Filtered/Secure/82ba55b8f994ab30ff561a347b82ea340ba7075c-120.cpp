BtnCode(XtermWidget xw, XButtonEvent *event, int button)
{
    int result = (int) (32 + (KeyState(xw, event->state) << 2));

    if (event->type == MotionNotify)
	result += 32;

    if (button < 0) {
	result += 3;
    } else {
	result += button & 3;
	if (button & 4)
	    result += 64;
	if (button & 8)
	    result += 128;
    }
    TRACE(("BtnCode button %d, %s state " FMT_MODIFIER_NAMES " ->%#x\n",
	   button,
	   visibleEventType(event->type),
	   ARG_MODIFIER_NAMES(event->state),
	   result));
    return result;
}