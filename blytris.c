#include <time.h>
#include <stdlib.h>

//piece order is ijlostz   ex:spawnlocx[0] holds the x coordinates for i piece spawn
//the coordinates are x,y coordinates starting from the axis and then from up to down, left to right, except i and o, which also end with another axis point

int board[10][25];
int piecex[4],piecey[4];
const int spawnlocx[7][4]={{4,3,6,5},{4,3,3,5},{4,5,3,5},{4,5,4,5},{4,4,5,3},{4,4,3,5},{4,3,4,5}};
const int spawnlocy[7][4]={{19,19,19,19},{19,20,19,19},{19,20,19,19},{20,19,19,20},{19,20,20,19},{19,20,19,19},{19,20,20,19}};
int activepiece;
int queue[14];
int queueaccesspoint;
int death=0;
int direction;
int rotdir;
int rotnum;
int lastrot;
const int kickx[5]={0,1, 1,0,1};
const int kicky[5]={0,0,-1,2,2};
const int kickydir[4]={1,-1,1,-1};
const int ikickx[5]={0,-2,1,-2,1};
const int ikickx2[5]={0,-1,2,-1,2};
const int ikicky[5]={0,0,0,-1,2};
const int ikicky2[5]={0,0,0,-2,1};
int holdpiece=-1;
int clearlines[4];
char input;

void BuildStack(){
	
	int r,rt;
	
	for (r=0;r<10;r++){
	
		for (rt=0;rt<25;rt++){
			
			board[r][rt]=0;
		}
	}
}

void GenBag(int* queue,int index){

	int randpieces[7]={0,1,2,3,4,5,6};
	int randomnumber;

	int r;

	//for(r=0;r<7;r++) randpieces[r]=r;
	
	for(r=7;r>0;r--){
		
		randomnumber=rand()%r;
		queue[index+r-1]=randpieces[randomnumber];
		randpieces[randomnumber]=randpieces[r-1];
	}
}

void LineClear(){
		
	int r,rt,rth;
		
	for(r=0;r<4;r++){
	
		for(rt=0;rt<10;rt++){
		
			if(board[rt][clearlines[r]]==0) break;
		}
		
		if(rt==10){
			
			for(rt=r+1;rt<4;rt++){
			
				if(clearlines[r]<clearlines[rt]) clearlines[rt]--;
			}
			
			for(rt=0;rt<10;rt++) board[rt][clearlines[r]]=0;
			
			for(rt=0;rt<10;rt++){
			
				for(rth=clearlines[r];rth<24;rth++) board[rt][rth]=board[rt][rth+1];
			}
			
			for(rt=0;rt<10;rt++) board[rt][24]=0;
		}
	}
}

void MoveQueue(){

	int r;

	queueaccesspoint++;
	
	if (queueaccesspoint==7){
	
		for (r=0;r<7;r++) queue[r]=queue[r+7];
		
		GenBag(queue,7);
		
		queueaccesspoint=0;
	}	
	activepiece=queue[queueaccesspoint];
}

void InitialQueue(){

	int r;

	queueaccesspoint=0;
	
	GenBag(queue,0);
		
	GenBag(queue,7);
	
	activepiece=queue[queueaccesspoint];
}

void SpawnPiece(){

	int r;

	for(r=0;r<4;r++){
		
		if(board[spawnlocx[activepiece][r]][spawnlocy[activepiece][r]]) death++;
		
		piecex[r]=spawnlocx[activepiece][r];
		piecey[r]=spawnlocy[activepiece][r];
	}
	rotnum=0;
	lastrot=0;
}

void MovePiece(){
	
	int r;
	
	for(r=0;r<4;r++){
	
		if(piecex[r]+direction==-1 || piecex[r]+direction==10) return;
		
		if(board[piecex[r]+direction][piecey[r]]!=0) return;
	}
		
	for(r=0;r<4;r++) piecex[r]=piecex[r]+direction;
}

void RotatePiece(){

	int tempkickx[5],tempkicky[5];
	int tempx[4],tempy[4];

	int r,rt;
	
	rotnum-=rotdir;
	
	if(rotnum==-1) rotnum=3;
	if(rotnum==4) rotnum=0;
	
	if(activepiece==0 || activepiece==3){
		
		int eaxis;
		
		if(rotdir==-1) eaxis=3;
		else eaxis=0;
		
		for(r=0;r<4;r++){
		
			if(eaxis==r){
			
				tempx[r]=piecex[eaxis];
				tempy[r]=piecey[eaxis];
			}
			else{
			
				tempx[r]=(piecey[eaxis]-piecey[r])*rotdir+piecex[eaxis];
				tempy[r]=(piecex[eaxis]-piecex[r])*rotdir*(-1)+piecey[eaxis];
			}
		}
		
		switch(rotnum){
			
			case 0:
				for(r=0;r<4;r++) tempx[r]-=rotdir;
			break;
		
			case 1:
				for(r=0;r<4;r++) tempy[r]+=rotdir;
			break;
			
			case 2:
				for(r=0;r<4;r++) tempx[r]+=rotdir;
			break;
			
			default:
				for(r=0;r<4;r++) tempy[r]-=rotdir;
		}		
			
		int idirx,idiry;
		
		if(lastrot==0 || rotnum==2) idirx=1;
		else idirx=-1;
		
		if(lastrot==3 || rotnum==1) idiry=1;
		else idiry=-1;
		
		if(
			(lastrot==1 && rotnum==0) ||
			(lastrot==2 && rotnum==3) ||
			(lastrot==0 && rotnum==1) ||
			(lastrot==3 && rotnum==2))
		{
		
			for(r=0;r<5;r++){
			
				tempkickx[r]=ikickx[r]*idirx;
				tempkicky[r]=ikicky[r]*idiry;
			}
		}
		else{
		
			for(r=0;r<5;r++){
			
				tempkickx[r]=ikickx2[r]*idirx;
				tempkicky[r]=ikicky2[r]*idiry;
			}
		}
	}
	else{
	
		int kickxdir;
	
		if(rotnum==1 || lastrot==3) kickxdir=-1;
		else kickxdir=1;
			
		tempx[0]=piecex[0];
		tempy[0]=piecey[0];
			
		for(r=1;r<4;r++){
			
			tempx[r]=(piecey[0]-piecey[r])*rotdir+piecex[0];
			tempy[r]=(piecex[0]-piecex[r])*rotdir*(-1)+piecey[0];
		}
			
		for(r=0;r<5;r++){
				
			tempkickx[r]=kickx[r]*kickxdir;
			tempkicky[r]=kicky[r]*kickydir[rotnum];
		}
	}
		
	for(r=0;r<5;r++){
		
		for(rt=0;rt<4;rt++){
		
			if(
				tempx[rt]+tempkickx[r]>9 ||
				tempx[rt]+tempkickx[r]<0 ||
				tempy[rt]+tempkicky[r]>24 ||
				tempy[rt]+tempkicky[r]<0 ||
				board[tempx[rt]+tempkickx[r]][tempy[rt]+tempkicky[r]]!=0)
			{break;}
		}
		
		if(rt==4){
	
			for(rt=0;rt<4;rt++){
			
				piecex[rt]=tempx[rt]+tempkickx[r];
				piecey[rt]=tempy[rt]+tempkicky[r];
			}
			
			lastrot=rotnum;
			
			return;
		}
	}
	
	rotnum=lastrot;
}

void HoldPiece(){

	if(holdpiece==-1){
	
		holdpiece=activepiece;
		MoveQueue();
		SpawnPiece();
	}
	else{
	
		activepiece=holdpiece;
		holdpiece=queue[queueaccesspoint];
		queue[queueaccesspoint]=activepiece;
		SpawnPiece();
	}
}

void HardDrop(){

	int r,rt;
	
	for(r=0;;r++){
	
		for(rt=0;rt<4;rt++){
			
			if(piecey[rt]-r==-1 || board[piecex[rt]][piecey[rt]-r]!=0){
			
				for(rt=0;rt<4;rt++){
				
					board[piecex[rt]][piecey[rt]-r+1]=activepiece+1;
					
					clearlines[rt]=piecey[rt]-r+1;
				}
				LineClear();
				MoveQueue();
				SpawnPiece();
				return;
			}
		}
	}
}

void SoftDrop(){

	int r;

	for(r=0;r<4;r++){
	
		if(board[piecex[r]][piecey[r]-1]!=0 || (piecey[r]-1)<0) return;
	}
	
	for(r=0;r<4;r++) piecey[r]--;
}

void main(){

	srand(time(0));
	BuildStack();
	InitialQueue();
	SpawnPiece();
	
	while(!death){
				
		/*
		get input
		decide which functions to call based off input
		for soft drop, SoftDrop();
		for clockwise, rotdir=-1; RotatePiece();
		for counter clockwise, rotdir=1; RotatePiece();
		for hold, HoldPiece();
		for left movement, direction=-1; MovePiece();
		for right movement, direction=1; MovePiece();
		for hard drop, HardDrop();
		to quit the game, either kill yourself or increment death when a key is pressed
		*/
	}
	//game over
}