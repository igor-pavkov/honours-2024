void callbacks_layer_tree_row_inserted (GtkTreeModel *tree_model, GtkTreePath  *path,
                              GtkTreeIter  *oIter, gpointer user_data) {
	gint *indices=NULL,oldPosition,newPosition;
      
	if ((!screen.win.treeIsUpdating)&&(path != NULL)) {
		indices = gtk_tree_path_get_indices (path);
		if (indices) {
			newPosition = indices[0];
			oldPosition = callbacks_get_selected_row_index ();
			/* compensate for the fact that the old row has already
			   been removed */
			if (oldPosition < newPosition)
				newPosition--;
			else
				oldPosition--;
			gerbv_change_layer_order (mainProject, oldPosition, newPosition);

			if (screenRenderInfo.renderType <= GERBV_RENDER_TYPE_GDK_XOR) {
				render_refresh_rendered_image_on_screen();
			}
			else {
				render_recreate_composite_surface (screen.drawing_area);
				callbacks_force_expose_event_for_screen ();
			}
			/* select the new line */
			GtkTreeSelection *selection;
			GtkTreeIter iter;
			GtkListStore *list_store = (GtkListStore *) gtk_tree_view_get_model
				((GtkTreeView *) screen.win.layerTree);
			
			selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(screen.win.layerTree));
			if (gtk_tree_model_get_iter ((GtkTreeModel *)list_store, &iter, path))
				gtk_tree_selection_select_iter (selection, &iter);
		}
	}
}