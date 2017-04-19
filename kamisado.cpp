#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <map>


enum{
INIT,
PLAYING,
END,
PLAYER1,
PLAYER2,
};

enum{
IA_RANDOM
};

enum{
START_POINT_PLAYER1=0,
START_POINT_PLAYER2=7
};
struct vertex{
    int x;
    int y;
};
struct movement{
    std::vector<int> identifiants;
    std::vector< std::vector<vertex> > coordonnes;
};

struct piece{
    int x,y;
    int id;
    int player;
    int color_piece;
    int color_ground;
};

class kamisado
{
    private:
        int etat;
        int turn;
        int winner;
        int intelligence;
        int lastGroundColor;
        piece pieceTable[8*8];
        piece simulation[64];
        movement totalMove;
    public:
        void initializeTable(void);
        kamisado(void);
        void printColorTable(void);
        void printPieceTable(void);
        std::vector<vertex> genMove(const piece &);
        void printMovement(movement);
        movement possible(piece *,int,int);
        void placer(piece * table,vertex,vertex);
        void randomMoveIA(int);
        void gameover(void);        
        void play(void);
        int dfs(piece *,int,int,int);
        int win(piece *,int);
        void playdfs(int);
};
void kamisado::initializeTable(void)
{
    FILE * f,* f2;
    char buf[100];char buf2[100];
    
    f=fopen("color.txt","r");
    f2=fopen("piece.txt","r");
    for(int i=0;i<8;i++)
    {
        fgets(buf,100,f);
        fgets(buf2,100,f2);
        for(int j=0;j<8;j++)
        {
         //   colorTable[j+i*8]=buf[j]-'0';        
            pieceTable[j+i*8].id=j+i*8;           
            pieceTable[j+i*8].x=j;           
            pieceTable[j+i*8].y=i;
 
 
            pieceTable[j+i*8].color_piece=buf2[j]-'0';
            pieceTable[j+i*8].color_ground=buf[j]-'0';
            if(i==START_POINT_PLAYER1)
                pieceTable[j+i*8].player=PLAYER1;
            else if(i==START_POINT_PLAYER2)
                pieceTable[j+i*8].player=PLAYER2;
            else
                pieceTable[j+i*8].player=0;
        }           
    }
    fclose(f);
    fclose(f2);

}

kamisado::kamisado(void)
{
    etat=INIT;
    turn=PLAYER1;
    winner=0;
    intelligence=IA_RANDOM;
    lastGroundColor=0;
    initializeTable();
}

void kamisado::printColorTable(void)
{
    std::cout<<"color table\n";
    for(int i=0;i<8;i++)
    {
        for(int j=0;j<8;j++)
        {
            std::cout<<pieceTable[j+i*8].color_ground<<" ";
        }           
        std::cout<<"\n";
    }

}
void kamisado::printPieceTable(void)
{

    std::cout<<"piece table\n";
    for(int i=0;i<8;i++)
    {
        for(int j=0;j<8;j++)
        {
            std::cout<<pieceTable[j+i*8].color_piece;
            if(pieceTable[j+i*8].player==PLAYER1)
                std::cout<<"A ";
            else if(pieceTable[j+i*8].player==PLAYER2)
               std::cout<<"B ";
            else
               std::cout<<"  ";
            
        }           
        std::cout<<"\n";
    }

}

std::vector<vertex> kamisado::genMove(const piece & p)
{
    std::vector<vertex> movementVector;
    int tabX[]={1,-1,1,-1,0,1,0,-1};
    int tabY[]={1,1,-1,-1,1,0,-1,0};

    for(int i=0;i<8;++i)
        for(int moveX=tabX[i],moveY=tabY[i];1;moveX+=tabX[i],moveY+=tabY[i])
            if(p.x+moveX<8&&p.x+moveX>=0&&p.y+moveY<8&&p.y+moveY>=0)
            {
                if(pieceTable[p.x+moveX+8*(p.y+moveY)].player==0)
                {
                    vertex v;    
                    v.x=p.x+moveX;
                    v.y=p.y+moveY;
                    movementVector.push_back(v);
                }else    
                    break;
            }else    
                break;

    return movementVector;
}


movement kamisado::possible(piece *table,int player,int lastColor)
{
    totalMove.identifiants.clear();
    totalMove.coordonnes.clear();
    for(int j=0;j<8;j++)
    {
        for(int i=0;i<8;i++)
        {
            if(table[i+j*8].player==player&&(table[i+j*8].color_piece==lastColor||0==lastColor))
            {
                std::vector<vertex> movementPossibles = genMove(table[i+j*8]);
                //if( movementPossibles.size()==0)
                {
                    totalMove.identifiants.push_back(table[i+j*8].id);
                    totalMove.coordonnes.push_back(movementPossibles);
                }            
            }
        }           
    }

    return totalMove;

}

void kamisado::placer(piece * table,vertex ancien,vertex nouveau)
{
    
    table[nouveau.x+8*nouveau.y].player=table[ancien.x+8*ancien.y].player;    
    table[nouveau.x+8*nouveau.y].color_piece=table[ancien.x+8*ancien.y].color_piece;    
    table[ancien.x+8*ancien.y].player=0;
    table[ancien.x+8*ancien.y].color_piece=0;
}

void kamisado::printMovement(movement mv)
{
    for(int i=0; i<mv.identifiants.size();i++)
    {
        std::cout<<mv.identifiants[i]<<"\n";    
        for(int j=0; j<mv.coordonnes[i].size();j++)
        {
            std::vector<vertex> tmp=mv.coordonnes[i];
            std::cout<<"  "<<tmp[j].x<<" "<<tmp[j].y<<"\n";    
        }
    }

}
void kamisado::randomMoveIA(int player)
{
        movement mv = possible(pieceTable,player,lastGroundColor);
        int randomNB=rand()%mv.identifiants.size();    
        int randomNB2=rand()%mv.coordonnes[randomNB].size();    
//        std::cout<<"  "<<mv.identifiants[0]<<" "<<pieceTable[mv.identifiants[0]].x<<" "<<pieceTable[mv.identifiants[0]].y<<"\n";    
//        std::cout<<"  "<<mv.identifiants[0]<<" "<<mv.coordonnes[0][0].x<<" "<<mv.coordonnes[0][0].y<<"\n";
        
        vertex v;
        v.x=pieceTable[mv.identifiants[randomNB]].x;
        v.y=pieceTable[mv.identifiants[randomNB]].y;

        lastGroundColor=pieceTable[mv.coordonnes[randomNB][randomNB2].x+8*mv.coordonnes[randomNB][randomNB2].y].color_ground;
        placer(pieceTable,v,mv.coordonnes[randomNB][randomNB2]);
}
void kamisado::gameover(void)
{
    for(int i=0;i<8;i++)
    {
        if(pieceTable[i+8*START_POINT_PLAYER1].player==PLAYER2)
        {
            winner=PLAYER2;
            etat=END;
        }    
        if(pieceTable[i+8*START_POINT_PLAYER2].player==PLAYER1)
        {
            winner=PLAYER1;
            etat=END;
        }    
    }

}

int kamisado::win(piece * table,int player)
{
    for(int i=0;i<8;i++)
    {
        if(player==PLAYER2)
        {           
            if(table[i+8*START_POINT_PLAYER1].player==PLAYER2)
            {
                return 1;
            }    
        }
        else
        {
            if(table[i+8*START_POINT_PLAYER2].player==PLAYER1)
            {
                return 1;
            }    
        }
        
    }
    return 0;
}


static int niveau=0;
int kamisado::dfs(piece * table,int turnPlayer,int player,int lastColor)
{
            std::cout<< niveau++;
//    if(niveau>4000)
//        return -1;        
    movement mv = possible(table,turnPlayer,lastColor);    
    for(int i=0; i<mv.identifiants.size();i++)
    {            
        std::vector<vertex> nouveau=mv.coordonnes[i];   
        vertex ancien = {table[mv.identifiants[i]].x,table[mv.identifiants[i]].y};
        for(int j=0; j<mv.coordonnes[i].size();j++)
        {
           // printPieceTable();
            placer(table,ancien,nouveau[j]);
            printPieceTable();
       
            if(win(table,player)==1)
                return j;
            int changeColor=table[nouveau[j].x+nouveau[j].y*8].color_ground;
            if(turnPlayer==PLAYER1)
            {
                if(dfs(table,PLAYER2,player,changeColor)!=-1)
                    return j;
  //              else
    //                return -1;   
            }
            else
            {
                if(dfs(table,PLAYER1,player,changeColor)!=-1)
                    return j; 
      //          else
        //            return -1; 
            }
            placer(table,nouveau[j],ancien);
        }
    }
    return -1;
}
/*
POUR TOUT
        placer()
        SI finish(table);
            RETURN WIN
        dfs(table,player2);
        remettre();        
    END

    RETURN NON
*/




void  kamisado::playdfs(int player){
    niveau=0;
    for(int i=0;i<64;i++)
    {
        simulation[i].x=pieceTable[i].x;
        simulation[i].y=pieceTable[i].y;
        simulation[i].id=pieceTable[i].id;
        simulation[i].player=pieceTable[i].player;
        simulation[i].color_piece=pieceTable[i].color_piece;
        simulation[i].color_ground=pieceTable[i].color_ground;
    }        
   int probable=dfs(pieceTable,player,player,lastGroundColor);
   
    for(int i=0;i<64;i++)
    {
        pieceTable[i].x=simulation[i].x;
        pieceTable[i].y=simulation[i].y;
        pieceTable[i].id=simulation[i].id;
        pieceTable[i].player=simulation[i].player;
        pieceTable[i].color_piece=simulation[i].color_piece;
        pieceTable[i].color_ground=simulation[i].color_ground;
    }        

    movement mv = possible(pieceTable,player,lastGroundColor);
    vertex ancien = {pieceTable[mv.identifiants[0]].x,pieceTable[mv.identifiants[0]].y};    
    lastGroundColor=pieceTable[mv.coordonnes[0][probable].x+8*mv.coordonnes[0][probable].y].color_ground;
    placer(pieceTable,ancien,mv.coordonnes[0][probable]);


}


void kamisado::play(void)
{
    while(etat!=END)    
    {
        if(intelligence==IA_RANDOM)
        {
            if(turn==PLAYER1)
            {
                playdfs(PLAYER1);            
                turn=PLAYER2;            
            }
            else
            {
                randomMoveIA(PLAYER2);            
                turn=PLAYER1;            
            }    
            
        }//printPieceTable();
       
        gameover();
    }    
    if(PLAYER1==winner)
        std::cout << "winner A\n";
    else if(PLAYER2==winner)
        std::cout << "winner B\n";
    

}
int main(){
    kamisado jeu;
    srand(time(NULL));
    jeu.printColorTable();
    jeu.printPieceTable();
    //jeu.printMovement(jeu.possible(PLAYER1));    
    //jeu.randomMoveIA(PLAYER1);
   // jeu.randomMoveIA(PLAYER2);
//    jeu.printMovement(jeu.possible(PLAYER2));    
// jeu.randomMoveIA(PLAYER1);
//    jeu.randomMoveIA(PLAYER2);
//    jeu.printColorTable();
    //jeu.printPieceTable();

// jeu.playdfs(PLAYER2);
// jeu.randomMoveIA(PLAYER2);
    jeu.play();
    jeu.printColorTable();
    jeu.printPieceTable();
    
}





