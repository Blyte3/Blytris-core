#include <time.h>
#include <stdlib.h>
#include "blytris.h"

#define ROWS 25
#define COLS 10

//piece order is ijlostz   ex:spawnlocx[0] holds the x coordinates for i piece spawn
//the coordinates are x,y coordinates starting from the axis and then from up to down, left to right, except i and o, which also end with another axis point

const int spawnlocx[7][4]={{4,3,6,5},{4,3,3,5},{4,5,3,5},{4,5,4,5},{4,4,5,3},{4,4,3,5},{4,3,4,5}};
const int spawnlocy[7][4]={{19,19,19,19},{19,20,19,19},{19,20,19,19},{20,19,19,20},{19,20,20,19},{19,20,19,19},{19,20,20,19}};
int death=0;;
const int kickx[5]={0,1, 1,0,1};
const int kicky[5]={0,0,-1,2,2};
const int kickydir[4]={1,-1,1,-1};
const int ikickx[5]={0,-2,1,-2,1};
const int ikickx2[5]={0,-1,2,-1,2};
const int ikicky[5]={0,0,0,-1,2};
const int ikicky2[5]={0,0,0,-2,1};
char input;

void resetboard(boardt *board){
	
	int r,rt;
	
	for (r=0;r<COLS;r++)
		for (rt=0;rt<ROWS;rt++)
			board->board[r][rt]=0;
	
	board->queue.queueindex=0;
	
	genbag(board->queue.queue,0);
	genbag(board->queue.queue,7);
	
	board->activepiece=board->queue.queue[board->queue.queueindex];
	board->isalive=1;
	board->queue.holdpiece=-1;
}

void genbag(pieces *queue,int index){

	pieces randpieces[7]={PIECE_O,PIECE_J,PIECE_L,PIECE_O,PIECE_S,PIECE_T,PIECE_Z};
	int randomnumber;

	int r;
	
	for(r=7;r>0;r--){
		
		randomnumber=rand()%r;
		queue[index+r-1]=randpieces[randomnumber];
		randpieces[randomnumber]=randpieces[r-1];
	}
}

void clearlines(boardt *board){
		
	int r,rt,rth;
		
	for(r=0;r<4;r++){
	
		for(rt=0;rt<COLS;rt++){
		
			if(board->board[rt][board->rowstocheck[r]]==0) break;
		}
		
		if(rt==COLS){
			
			for(rt=r+1;rt<4;rt++){
			
				if(board->rowstocheck[r]<board->rowstocheck[rt]) board->rowstocheck[rt]--;
			}
			
			for(rt=0;rt<COLS;rt++){
			
				for(rth=board->rowstocheck[r];rth<(ROWS-1);rth++) board->board[rt][rth]=board->board[rt][rth+1];
				
				board->board[rt][ROWS-1]=0;
			}
		}
	}
}

void movequeue(boardt *board){

	int r;

	board->queue.queueindex++;
	
	if (board->queue.queueindex==7){
	
		for (r=0;r<7;r++) board->queue.queue[r]=board->queue.queue[r+7];
		
		genbag(board->queue.queue,7);
		
		board->queue.queueindex=0;
	}	
	board->activepiece=board->queue.queue[board->queue.queueindex];
}

void spawnnewpiece(boardt *board){

	int r;

	for(r=0;r<4;r++){
		
		if(board->board[spawnlocx[board->activepiece][r]][spawnlocy[board->activepiece][r]]) board->isalive=0;
		
		board->piece.piecex[r]=spawnlocx[board->activepiece][r];
		board->piece.piecey[r]=spawnlocy[board->activepiece][r];
	}
	board->piece.rotationnum=0;
	board->piece.lastrot=0;
}

void movepiece(boardt *board,int xchange,int ychange){
	
	int r;
	
	for(r=0;r<4;r++){
	
		if(board->piece.piecex[r]+xchange<0
		|| board->piece.piecex[r]+xchange>(COLS-1)
		|| board->piece.piecey[r]+ychange<0
		|| board->piece.piecey[r]+ychange>(ROWS-1)
		) return;
		
		if(board->board[board->piece.piecex[r]+xchange][board->piece.piecey[r]+ychange]!=0) return;
	}
		
	for(r=0;r<4;r++){
		
		board->piece.piecex[r]=board->piece.piecex[r]+xchange;
		board->piece.piecey[r]=board->piece.piecey[r]+ychange;
	}
}

void rotatepiece(boardt *board,int rotdir){

	char tempkickx[5],tempkicky[5];
	char tempx[4],tempy[4];

	int r,rt;
	
	board->piece.rotationnum-=rotdir;
	
	if(board->piece.rotationnum<0) board->piece.rotationnum=3;
	if(board->piece.rotationnum>3) board->piece.rotationnum=0;
	
	if(board->activepiece==PIECE_I || board->activepiece==PIECE_O){
		
		int iaxis;
		
		if(rotdir==-1) iaxis=3;
		else iaxis=0;
		
		for(r=0;r<4;r++){
			
			tempx[r]=(board->piece.piecey[iaxis]-board->piece.piecey[r])*rotdir+board->piece.piecex[iaxis];
			tempy[r]=(board->piece.piecex[iaxis]-board->piece.piecex[r])*(rotdir*(-1))+board->piece.piecey[iaxis];
		}
		
		switch(board->piece.rotationnum){
			
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
		
		//prepare kick table

		int idirx,idiry;
		
		if(board->piece.lastrot==0 || board->piece.rotationnum==2) idirx=1;
		else idirx=-1;
		
		if(board->piece.lastrot==3 || board->piece.rotationnum==1) idiry=1;
		else idiry=-1;
		
		if(
			(board->piece.lastrot==1 && board->piece.rotationnum==0) ||
			(board->piece.lastrot==2 && board->piece.rotationnum==3) ||
			(board->piece.lastrot==0 && board->piece.rotationnum==1) ||
			(board->piece.lastrot==3 && board->piece.rotationnum==2))
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
	
		if(board->piece.rotationnum==1 || board->piece.lastrot==3) kickxdir=-1;
		else kickxdir=1;
			
		for(r=0;r<4;r++){
			
			tempx[r]=(board->piece.piecey[0]-board->piece.piecey[r])*rotdir+board->piece.piecex[0];
			tempy[r]=(board->piece.piecex[0]-board->piece.piecex[r])*rotdir*(-1)+board->piece.piecey[0];
		}
			
		for(r=0;r<5;r++){
				
			tempkickx[r]=kickx[r]*kickxdir;
			tempkicky[r]=kicky[r]*kickydir[board->piece.rotationnum];
		}
	}
		
	for(r=0;r<5;r++){
		
		for(rt=0;rt<4;rt++){
		
			if(tempx[rt]+tempkickx[r]>9
				||tempx[rt]+tempkickx[r]<0
				||tempy[rt]+tempkicky[r]>24
				||tempy[rt]+tempkicky[r]<0
				||board->board[tempx[rt]+tempkickx[r]][tempy[rt]+tempkicky[r]]!=0)
			break;
		}
		
		if(rt==4){
	
			for(rt=0;rt<4;rt++){
			
				board->piece.piecex[rt]=tempx[rt]+tempkickx[r];
				board->piece.piecey[rt]=tempy[rt]+tempkicky[r];
			}
			
			board->piece.lastrot=board->piece.rotationnum;
			
			return;
		}
	}
	
	board->piece.rotationnum=board->piece.lastrot;
}

void holdpiece(boardt *board){

	if(board->queue.holdpiece==-1){
	
		board->queue.holdpiece=board->activepiece;
		movequeue(board);
		spawnnewpiece(board);
	}
	else{
	
		board->activepiece=board->queue.holdpiece;
		board->queue.holdpiece=board->queue.queue[board->queue.queueindex];
		board->queue.queue[board->queue.queueindex]=board->activepiece;
		spawnnewpiece(board);
	}
}

void harddrop(boardt *board){

	int r,rt;
	
	for(r=0;;r++){
	
		for(rt=0;rt<4;rt++){
			
			if(board->piece.piecey[rt]-r==-1 || board->board[board->piece.piecex[rt]][board->piece.piecey[rt]-r]!=0){
			
				for(rt=0;rt<4;rt++){
				
					board->board[board->piece.piecex[rt]][board->piece.piecey[rt]-r+1]=board->activepiece+1;
					
					board->rowstocheck[rt]=board->piece.piecey[rt]-r+1;
				}
				clearlines(board);
				movequeue(board);
				spawnnewpiece(board);
				return;
			}
		}
	}
}

void main(){

	boardt board;
	
	srand(time(0));
	resetboard(&board);
	spawnnewpiece(&board);
	
	while(1){
		
		break;
	}
}
