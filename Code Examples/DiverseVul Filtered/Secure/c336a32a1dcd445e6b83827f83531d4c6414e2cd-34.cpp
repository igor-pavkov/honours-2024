void WNewAutoFlow(Window *window, int on)
{
	if (window->w_flow & FLOW_AUTOFLAG)
		window->w_flow = FLOW_AUTOFLAG | (FLOW_AUTO | FLOW_NOW) * on;
	else
		window->w_flow = (window->w_flow & ~FLOW_AUTO) | FLOW_AUTO * on;
	LSetFlow(&window->w_layer, window->w_flow & FLOW_NOW);
}