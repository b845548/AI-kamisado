#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <map>
#ifdef AI

#else
#include <GL4D/gl4dp.h>
#include <GL4D/gl4duw_SDL2.h>	
#endif

enum{
INIT,
PLAYER1,
PLAYER2,
END,
IA_RANDOM
};

#define WIDTH 8
#define HEIGHT 8
#define START_POINT_PLAYER1 0
#define START_POINT_PLAYER2 7


struct vertex
{
    int x;
    int y;
};

struct movement
{
    std::vector<int> identifiants;
    std::vector< std::vector<vertex> > coordonnes;
};

struct piece
{
    int x,y;
    int player;
    int color_piece;
    int color_ground;
};

struct mcts_node
{
    int id;
    int parent;
    int player; 
    int ground_color;
    int total_play;
    int win_play;
	std::pair<vertex,vertex> move;
    std::vector<int>adjacent;
    piece table[WIDTH*HEIGHT];
};


class kamisado
{
    private:
        int playerAI;
        int iteration;
        int etat;
        int turn;
        int winner;
        int intelligence;
        int lastGroundColor;
        piece pieceTable[WIDTH*HEIGHT];
        piece simulation[WIDTH*HEIGHT];
        movement totalMove;
        int nodeId;
        std::map<int,mcts_node> arbre;

    public:
        void initializeTable(void);
        kamisado(void);
        void printColorTable(void);
        void printPieceTable(void);
        std::vector<vertex> genMove(const piece &,piece * table);
        void printMovement(movement);
        movement possible(piece *,int,int);
        void placer(piece * table,vertex,vertex);
        int gameover(void);        
        void play(void);
        void playStep(void);        
        void draw(void);
        void RANDOM_AI(int);
        int DFS_AI(int);
        std::pair<int,int> DFS_recursive(piece *,int,int,int);
        int DFS_win(piece *,int);
        std::string MCTS_AI(int);	
		void MCTS_initialize(int);
		std::pair<int,double> MCTS_selection(int, std::pair<int,double>);
		int MCTS_expansion(int);
		int MCTS_simulation(int);
		void MCTS_retroPropagation(int,int,int);
		void MCTS_printArbre(int);
		bool AI_Play(std::vector<std::string> command);

};
