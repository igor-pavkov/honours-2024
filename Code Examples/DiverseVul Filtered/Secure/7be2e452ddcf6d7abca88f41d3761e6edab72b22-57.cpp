void t2p_compose_pdf_page_orient(T2P_BOX* boxp, uint16 orientation){

	float m1[9];
	float f=0.0;
	
	if( boxp->x1 > boxp->x2){
		f=boxp->x1;
		boxp->x1=boxp->x2;
		boxp->x2 = f;
	}
	if( boxp->y1 > boxp->y2){
		f=boxp->y1;
		boxp->y1=boxp->y2;
		boxp->y2 = f;
	}
	boxp->mat[0]=m1[0]=boxp->x2-boxp->x1;
	boxp->mat[1]=m1[1]=0.0;
	boxp->mat[2]=m1[2]=0.0;
	boxp->mat[3]=m1[3]=0.0;
	boxp->mat[4]=m1[4]=boxp->y2-boxp->y1;
	boxp->mat[5]=m1[5]=0.0;
	boxp->mat[6]=m1[6]=boxp->x1;
	boxp->mat[7]=m1[7]=boxp->y1;
	boxp->mat[8]=m1[8]=1.0;
	switch(orientation){
		case 0:
		case 1:
			break;
		case 2:
			boxp->mat[0]=0.0F-m1[0];
			boxp->mat[6]+=m1[0];
			break;
		case 3:
			boxp->mat[0]=0.0F-m1[0];
			boxp->mat[4]=0.0F-m1[4];
			boxp->mat[6]+=m1[0];
			boxp->mat[7]+=m1[4];
			break;
		case 4:
			boxp->mat[4]=0.0F-m1[4];
			boxp->mat[7]+=m1[4];
			break;
		case 5:
			boxp->mat[0]=0.0F;
			boxp->mat[1]=0.0F-m1[0];
			boxp->mat[3]=0.0F-m1[4];
			boxp->mat[4]=0.0F;
			boxp->mat[6]+=m1[4];
			boxp->mat[7]+=m1[0];
			break;
		case 6:
			boxp->mat[0]=0.0F;
			boxp->mat[1]=0.0F-m1[0];
			boxp->mat[3]=m1[4];
			boxp->mat[4]=0.0F;
			boxp->mat[7]+=m1[0];
			break;
		case 7:
			boxp->mat[0]=0.0F;
			boxp->mat[1]=m1[0];
			boxp->mat[3]=m1[4];
			boxp->mat[4]=0.0F;
			break;
		case 8:
			boxp->mat[0]=0.0F;
			boxp->mat[1]=m1[0];
			boxp->mat[3]=0.0F-m1[4];
			boxp->mat[4]=0.0F;
			boxp->mat[6]+=m1[4];
			break;
	}

	return;
}