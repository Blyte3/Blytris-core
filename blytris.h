
#define ROWS 25
#define COLS 10

typedef enum{
	
	EMPTYSPACE,
	PIECE_I,
	PIECE_J,
	PIECE_L,
	PIECE_O,
	PIECE_S,
	PIECE_T,
	PIECE_Z,
	GARBAGESPACE
}pieces;

typedef struct{
	
	char piecex[4];
	char piecey[4];
	char rotationnum;
	char lastrot;
	
}piecet;

typedef struct{
	
	pieces queue[14];
	char queueindex;
	pieces holdpiece;

}queuet;

typedef struct{
	
	pieces board[COLS][ROWS];
	piecet piece;
	queuet queue;
	pieces activepiece;
	char rowstocheck[4];
	unsigned int isalive:1;
	
}boardt;

void resetboard(boardt *board);
void genbag(pieces *queue,int index);
void clearlines(boardt *board);
void movequeue(boardt *board);
void spawnnewpiece(boardt *board);
void movepiece(boardt *board,int xchange,int ychange);
void rotatepiece(boardt *board,int rotdir);
void holdpiece(boardt *board);
void harddrop(boardt *board);
