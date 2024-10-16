void input_thread( void *arg) {

    if(!arg){}

    while( G.do_exit == 0 ) {
	int keycode=0;

	keycode=mygetch();

	if(keycode == KEY_s) {
	    G.sort_by++;
	    G.selection_ap = 0;
	    G.selection_sta = 0;

	    if(G.sort_by > MAX_SORT)
		G.sort_by = 0;

	    switch(G.sort_by) {
		case SORT_BY_NOTHING:
		    snprintf(G.message, sizeof(G.message), "][ sorting by first seen");
		    break;
		case SORT_BY_BSSID:
		    snprintf(G.message, sizeof(G.message), "][ sorting by bssid");
		    break;
		case SORT_BY_POWER:
		    snprintf(G.message, sizeof(G.message), "][ sorting by power level");
		    break;
		case SORT_BY_BEACON:
		    snprintf(G.message, sizeof(G.message), "][ sorting by beacon number");
		    break;
		case SORT_BY_DATA:
		    snprintf(G.message, sizeof(G.message), "][ sorting by number of data packets");
		    break;
		case SORT_BY_PRATE:
		    snprintf(G.message, sizeof(G.message), "][ sorting by packet rate");
		    break;
		case SORT_BY_CHAN:
		    snprintf(G.message, sizeof(G.message), "][ sorting by channel");
		    break;
		case SORT_BY_MBIT:
		    snprintf(G.message, sizeof(G.message), "][ sorting by max data rate");
		    break;
		case SORT_BY_ENC:
		    snprintf(G.message, sizeof(G.message), "][ sorting by encryption");
		    break;
		case SORT_BY_CIPHER:
		    snprintf(G.message, sizeof(G.message), "][ sorting by cipher");
		    break;
		case SORT_BY_AUTH:
		    snprintf(G.message, sizeof(G.message), "][ sorting by authentication");
		    break;
		case SORT_BY_ESSID:
		    snprintf(G.message, sizeof(G.message), "][ sorting by ESSID");
		    break;
		default:
		    break;
	    }
	    pthread_mutex_lock( &(G.mx_sort) );
		dump_sort();
	    pthread_mutex_unlock( &(G.mx_sort) );
	}

	if(keycode == KEY_SPACE) {
	    G.do_pause = (G.do_pause+1)%2;
	    if(G.do_pause) {
		snprintf(G.message, sizeof(G.message), "][ paused output");
		pthread_mutex_lock( &(G.mx_print) );

		    fprintf( stderr, "\33[1;1H" );
		    dump_print( G.ws.ws_row, G.ws.ws_col, G.num_cards );
		    fprintf( stderr, "\33[J" );
		    fflush(stderr);

		pthread_mutex_unlock( &(G.mx_print) );
	    }
	    else
		snprintf(G.message, sizeof(G.message), "][ resumed output");
	}

	if(keycode == KEY_r) {
	    G.do_sort_always = (G.do_sort_always+1)%2;
	    if(G.do_sort_always)
		snprintf(G.message, sizeof(G.message), "][ realtime sorting activated");
	    else
		snprintf(G.message, sizeof(G.message), "][ realtime sorting deactivated");
	}

	if(keycode == KEY_m) {
	    G.mark_cur_ap = 1;
	}

	if(keycode == KEY_ARROW_DOWN) {
	    if(G.selection_ap == 1) {
		G.selected_ap++;
	    }
	    if(G.selection_sta == 1) {
		G.selected_sta++;
	    }
	}

	if(keycode == KEY_ARROW_UP) {
	    if(G.selection_ap == 1) {
		G.selected_ap--;
		if(G.selected_ap < 1)
		    G.selected_ap = 1;
	    }
	    if(G.selection_sta == 1) {
		G.selected_sta--;
		if(G.selected_sta < 1)
		    G.selected_sta = 1;
	    }
	}

	if(keycode == KEY_i) {
	    G.sort_inv*=-1;
	    if(G.sort_inv < 0)
		snprintf(G.message, sizeof(G.message), "][ inverted sorting order");
	    else
		snprintf(G.message, sizeof(G.message), "][ normal sorting order");
	}

	if(keycode == KEY_TAB) {
	    if(G.selection_ap == 0) {
		G.selection_ap = 1;
		G.selected_ap = 1;
		snprintf(G.message, sizeof(G.message), "][ enabled AP selection");
		G.sort_by = SORT_BY_NOTHING;
	    } else if(G.selection_ap == 1) {
		G.selection_ap = 0;
		G.sort_by = SORT_BY_NOTHING;
		snprintf(G.message, sizeof(G.message), "][ disabled selection");
	    }
	}

	if(keycode == KEY_a) {
	    if(G.show_ap == 1 && G.show_sta == 1 && G.show_ack == 0) {
		G.show_ap = 1;
		G.show_sta = 1;
		G.show_ack = 1;
		snprintf(G.message, sizeof(G.message), "][ display ap+sta+ack");
	    } else if(G.show_ap == 1 && G.show_sta == 1 && G.show_ack == 1) {
		G.show_ap = 1;
		G.show_sta = 0;
		G.show_ack = 0;
		snprintf(G.message, sizeof(G.message), "][ display ap only");
	    } else if(G.show_ap == 1 && G.show_sta == 0 && G.show_ack == 0) {
		G.show_ap = 0;
		G.show_sta = 1;
		G.show_ack = 0;
		snprintf(G.message, sizeof(G.message), "][ display sta only");
	    } else if(G.show_ap == 0 && G.show_sta == 1 && G.show_ack == 0) {
		G.show_ap = 1;
		G.show_sta = 1;
		G.show_ack = 0;
		snprintf(G.message, sizeof(G.message), "][ display ap+sta");
	    }
	}

	if (keycode == KEY_d) {
		resetSelection();
		snprintf(G.message, sizeof(G.message), "][ reset selection to default");
	}

	if(G.do_exit == 0 && !G.do_pause) {
	    pthread_mutex_lock( &(G.mx_print) );

		fprintf( stderr, "\33[1;1H" );
		dump_print( G.ws.ws_row, G.ws.ws_col, G.num_cards );
		fprintf( stderr, "\33[J" );
		fflush(stderr);

	    pthread_mutex_unlock( &(G.mx_print) );
	}
    }
}