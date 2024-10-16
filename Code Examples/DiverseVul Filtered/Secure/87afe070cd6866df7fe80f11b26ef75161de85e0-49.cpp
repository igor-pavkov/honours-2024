void PrintDASHUsage()
{
	u32 i=0;
	gf_sys_format_help(helpout, help_flags, "# DASH Options\n"
		"Also see:\n"
		"- the [dasher `gpac -h dash`](dasher) filter documentation\n"
		"- [[DASH wiki|DASH-intro]].\n"
		"\n"
		"# Specifying input files\n"
		"Input media files to dash can use the following modifiers\n"
		"- #trackID=N: only use the track ID N from the source file\n"
		"- #N: only use the track ID N from the source file (mapped to [-tkid](mp4dmx))\n"
		"- #video: only use the first video track from the source file\n"
		"- #audio: only use the first audio track from the source file\n"
		"- :id=NAME: set the representation ID to NAME. Reserved value `NULL` disables representation ID for multiplexed inputs. If not set, a default value is computed and all selected tracks from the source will be in the same output mux.\n"
		"- :dur=VALUE: process VALUE seconds from the media. If VALUE is longer than media duration, last sample duration is extended.\n"
		"- :period=NAME: set the representation's period to NAME. Multiple periods may be used. Periods appear in the MPD in the same order as specified with this option\n"
		"- :BaseURL=NAME: set the BaseURL. Set multiple times for multiple BaseURLs\nWarning: This does not modify generated files location (see segment template).\n"
		"- :bandwidth=VALUE: set the representation's bandwidth to a given value\n"
		"- :pdur=VALUE: increase the duration of this period by the given duration in seconds (alias for period_duration:VALUE). This is only used when no input media is specified (remote period insertion), eg `:period=X:xlink=Z:pdur=Y`\n"
		"- :duration=VALUE: override target DASH segment duration for this input\n"
		"- :xlink=VALUE: set the xlink value for the period containing this element. Only the xlink declared on the first rep of a period will be used\n"
		"- :asID=VALUE: set the AdaptationSet ID to NAME\n"
		"- :role=VALUE: set the role of this representation (cf DASH spec). Media with different roles belong to different adaptation sets.\n"
		"- :desc_p=VALUE: add a descriptor at the Period level. Value must be a properly formatted XML element.\n"
		"- :desc_as=VALUE: add a descriptor at the AdaptationSet level. Value must be a properly formatted XML element. Two input files with different values will be in different AdaptationSet elements.\n"
		"- :desc_as_c=VALUE: add a descriptor at the AdaptationSet level. Value must be a properly formatted XML element. Value is ignored while creating AdaptationSet elements.\n"
		"- :desc_rep=VALUE: add a descriptor at the Representation level. Value must be a properly formatted XML element. Value is ignored while creating AdaptationSet elements.\n"
		"- :sscale: force movie timescale to match media timescale of the first track in the segment.\n"
		"- :trackID=N: only use the track ID N from the source file\n"
		"- @f1[:args][@fN:args][@@fK:args]: set a filter chain to insert between the source and the dasher. Each filter in the chain is formatted as a regular filter, see [filter doc `gpac -h doc`](filters_general). If several filters are set:\n"
		"  - they will be chained in the given order if separated by a single `@`\n"
		"  - a new filter chain will be created if separated by a double `@@`. In this case, no representation ID is assigned to the source.\n"
		"EX source.mp4:@enc:c=avc:b=1M@@enc:c=avc:b=500k\n"
		"This will load a filter chain with two encoders connected to the source and to the dasher.\n"
		"EX source.mp4:@enc:c=avc:b=1M@enc:c=avc:b=500k\n"
		"This will load a filter chain with the second encoder connected to the output of the first (!!).\n"
		"\n"
		"Note: `@f` must be placed after all other options.\n"
		"\n"
		"# Options\n"
		);


	while (m4b_dash_args[i].name) {
		GF_GPACArg *arg = (GF_GPACArg *) &m4b_dash_args[i];
		i++;
		gf_sys_print_arg(helpout, help_flags, arg, "mp4box-dash");
	}
}