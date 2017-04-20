
#include"kamisado.h"

#include<assert.h>






void kamisado::initializeTable(void)
{
    FILE * f,* f2;
    char buf[100];char buf2[100];
    etat=INIT;
    turn=PLAYER1;
    winner=0;
    intelligence=IA_RANDOM;
    lastGroundColor=0;
    
    f=fopen("color.txt","r");
    f2=fopen("piece.txt","r");
    for(int i=0;i<WIDTH;i++)
    {
        fgets(buf,100,f);
        fgets(buf2,100,f2);
        for(int j=0;j<WIDTH;j++)
        {  
            pieceTable[j+i*WIDTH].x=j;           
            pieceTable[j+i*WIDTH].y=i;
 
 
            pieceTable[j+i*WIDTH].color_piece=buf2[j]-'0';
            pieceTable[j+i*WIDTH].color_ground=buf[j]-'0';
            if(i==START_POINT_PLAYER1)
                pieceTable[j+i*WIDTH].player=PLAYER1;
            else if(i==START_POINT_PLAYER2)
                pieceTable[j+i*WIDTH].player=PLAYER2;
            else
                pieceTable[j+i*WIDTH].player=0;
        }           
    }
    fclose(f);
    fclose(f2);

}

kamisado::kamisado(void)
{
    initializeTable();
}

void kamisado::printColorTable(void)
{
    std::cout<<"color table\n";
    for(int i=0;i<WIDTH;i++)
    {
        for(int j=0;j<WIDTH;j++)
        {
            std::cout<<pieceTable[j+i*WIDTH].color_ground<<" ";
        }           
        std::cout<<"\n";
    }

}
void kamisado::printPieceTable(void)
{

    std::cout<<"piece table\n";
    for(int i=0;i<WIDTH;i++)
    {
        for(int j=0;j<WIDTH;j++)
        {
            std::cout<<pieceTable[j+i*WIDTH].color_piece;
            if(pieceTable[j+i*WIDTH].player==PLAYER1)
                std::cout<<"A ";
            else if(pieceTable[j+i*WIDTH].player==PLAYER2)
               std::cout<<"B ";
            else
               std::cout<<"  ";
            
        }           
        std::cout<<"\n";
    }

}

std::vector<vertex> kamisado::genMove(const piece & p,piece * table)
{
    std::vector<vertex> movementVector;
    int tabX[]={1,-1,1,-1,0,1,0,-1};
    int tabY[]={1,1,-1,-1,1,0,-1,0};

    for(int i=0;i<WIDTH;++i)
        for(int moveX=tabX[i],moveY=tabY[i];1;moveX+=tabX[i],moveY+=tabY[i])
            if(p.x+moveX<WIDTH&&p.x+moveX>=0&&p.y+moveY<WIDTH&&p.y+moveY>=0)
            {
                if(table[p.x+moveX+WIDTH*(p.y+moveY)].player==0)
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
    for(int j=0;j<WIDTH;j++)
    {
        for(int i=0;i<WIDTH;i++)
        {
            if(table[i+j*WIDTH].player==player&&(table[i+j*WIDTH].color_piece==lastColor||0==lastColor))
            {
                std::vector<vertex> movementPossibles = genMove(table[i+j*WIDTH],table);
                //if( movementPossibles.size()==0)
                {
                    int id=table[i+j*WIDTH].x+table[i+j*WIDTH].y*WIDTH;
                    totalMove.identifiants.push_back(id);
                    totalMove.coordonnes.push_back(movementPossibles);
                }            
            }
        }           
    }

    return totalMove;

}

void kamisado::placer(piece * table,vertex ancien,vertex nouveau)
{
    
    table[nouveau.x+WIDTH*nouveau.y].player=table[ancien.x+WIDTH*ancien.y].player;    
    table[nouveau.x+WIDTH*nouveau.y].color_piece=table[ancien.x+WIDTH*ancien.y].color_piece;    
    table[ancien.x+WIDTH*ancien.y].player=0;
    table[ancien.x+WIDTH*ancien.y].color_piece=0;
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
void kamisado::RANDOM_AI(int player)
{
        movement mv = possible(pieceTable,player,lastGroundColor);
        int randomNB=rand()%mv.identifiants.size();    
        int randomNB2=rand()%mv.coordonnes[randomNB].size();    
//        std::cout<<"  "<<mv.identifiants[0]<<" "<<pieceTable[mv.identifiants[0]].x<<" "<<pieceTable[mv.identifiants[0]].y<<"\n";    
//        std::cout<<"  "<<mv.identifiants[0]<<" "<<mv.coordonnes[0][0].x<<" "<<mv.coordonnes[0][0].y<<"\n";
        
        vertex v;
        v.x=pieceTable[mv.identifiants[randomNB]].x;
        v.y=pieceTable[mv.identifiants[randomNB]].y;

        lastGroundColor=pieceTable[mv.coordonnes[randomNB][randomNB2].x+WIDTH*mv.coordonnes[randomNB][randomNB2].y].color_ground;
        placer(pieceTable,v,mv.coordonnes[randomNB][randomNB2]);
}
void kamisado::gameover(void)
{
    for(int i=0;i<WIDTH;i++)
    {
        if(pieceTable[i+WIDTH*START_POINT_PLAYER1].player==PLAYER2)
        {
            winner=PLAYER2;
            etat=END;
        }    
        if(pieceTable[i+WIDTH*START_POINT_PLAYER2].player==PLAYER1)
        {
            winner=PLAYER1;
            etat=END;
        }    
    }

}

int kamisado::DFS_win(piece * table,int player)
{
    for(int i=0;i<WIDTH;i++)
    {
            if(table[i+WIDTH*START_POINT_PLAYER1].player==PLAYER2)
            {
                if(player==PLAYER2)
                    return 1;
                else    
                    return 0;

;
                    
            }    
            if(table[i+WIDTH*START_POINT_PLAYER2].player==PLAYER1)
            {
                if(player==PLAYER1)
                    return 1;
                else    
                    return 0;
            }    
        
    }
    return 0;
}


std::pair<int,int> kamisado::DFS_recursive(piece * table,int turnPlayer,int player,int lastColor)
{
    if(iteration++>5000)
        return (std::pair<int,int>){0,-1};        
    movement mv = possible(table,turnPlayer,lastColor);    
    for(int i=0; i<mv.identifiants.size();i++)
    {            
        std::vector<vertex> nouveau=mv.coordonnes[i];   
        vertex ancien = {table[mv.identifiants[i]].x,table[mv.identifiants[i]].y};
        for(int j=0; j<mv.coordonnes[i].size();j++)
        {
           // printPieceTable();
            placer(table,ancien,nouveau[j]);
            if(DFS_win(table,player)==1)
                return (std::pair<int,int>){i,j};
            int changeColor=table[nouveau[j].x+nouveau[j].y*WIDTH].color_ground;
            if(turnPlayer==PLAYER1)
            {
                if(DFS_recursive(table,PLAYER2,player,changeColor).second!=-1)
                    return (std::pair<int,int>){i,j};
                else
                    return (std::pair<int,int>){0,-1};   
            }
            else
            {
                if(DFS_recursive(table,PLAYER1,player,changeColor).second!=-1)
                    return (std::pair<int,int>){i,j};
                else
                    return (std::pair<int,int>){0,-1};
            }
            placer(table,nouveau[j],ancien);
        }
    }
    return (std::pair<int,int>){0,-1};
}
/*
POUR TOUT
        placer()
        SI finish(table);
            RETURN WIN
        DFS_recursive(table,player2);
        remettre();        
    END

    RETURN NON
*/




int kamisado::DFS_AI(int player){
    iteration=0;
    for(int i=0;i<WIDTH*HEIGHT;i++)
    {
        simulation[i].x=pieceTable[i].x;
        simulation[i].y=pieceTable[i].y;
        simulation[i].player=pieceTable[i].player;
        simulation[i].color_piece=pieceTable[i].color_piece;
        simulation[i].color_ground=pieceTable[i].color_ground;
    }       
    std::pair<int,int> probable=DFS_recursive(pieceTable,player,player,lastGroundColor);
   
    for(int i=0;i<WIDTH*HEIGHT;i++)
    {
        pieceTable[i].x=simulation[i].x;
        pieceTable[i].y=simulation[i].y;
        pieceTable[i].player=simulation[i].player;
        pieceTable[i].color_piece=simulation[i].color_piece;
        pieceTable[i].color_ground=simulation[i].color_ground;
    }        

    movement mv = possible(pieceTable,player,lastGroundColor);
    vertex ancien = {pieceTable[mv.identifiants[probable.first]].x,pieceTable[mv.identifiants[probable.first]].y};    
    if(probable.second==-1)
    {
        
        if(mv.coordonnes[0].size()==0)return -1;
        int randomNB=(int)rand()%mv.coordonnes[0].size();
        lastGroundColor=pieceTable[mv.coordonnes[0][randomNB].x+WIDTH*mv.coordonnes[0][randomNB].y].color_ground;
        placer(pieceTable,ancien,mv.coordonnes[0][randomNB]);
    }
    else
    {
        lastGroundColor=pieceTable[mv.coordonnes[probable.first][probable.second].x+WIDTH*mv.coordonnes[probable.first][probable.second].y].color_ground;
        placer(pieceTable,ancien,mv.coordonnes[probable.first][probable.second]);
    }       
    

}


void kamisado::play(void)
{
    while(etat!=END)    
    {
            if(turn==PLAYER1)
            {
                MCTS_AI(PLAYER1);            
                turn=PLAYER2;            
            }
            else
            {
                if(DFS_AI(PLAYER2)==-1){
                    std::cout << "match null\n";                
                break;            
                }
                turn=PLAYER1;            
            }    
            
        gameover();
        draw();
    }    
    if(PLAYER1==winner)
        std::cout << "winner MCTS\n";
    else if(PLAYER2==winner)
        std::cout << "winner DFS\n";
    

}
void rectangle(int x, int y, int w,int h)
{
    for(int i=0;i<h;i++)
    {
        for(int j=0;j<w;j++)
        {
            gl4dpPutPixel(x+i,y+j);
        }
    }

}
struct color{
int r,g,b;
};
color colorTable[]={{255,255,255},{255,99,71},{0,0,255},{32,112,255},{255,222,193},{255,255,0},{255,0,0},{0,255,0},{78,46,40}};
void kamisado::draw(void)
{
    int padX=20,padY=20;
    int rect=50;

//    gl4dpClearScreenWith (RGB(colorTable[], 0, 0));
    gl4dpUpdateScreen(NULL);
   
    for(int i=0;i<HEIGHT;i++)
    {
        for(int j=0;j<WIDTH;j++)
        {
            gl4dpSetColor(RGB(colorTable[pieceTable[j+i*8].color_ground].r,colorTable[pieceTable[j+i*8].color_ground].g,colorTable[pieceTable[j+i*8].color_ground].b));
            rectangle(padX+j*60,padY+i*60,rect,rect);
  
            if(pieceTable[j+i*8].player==PLAYER1)
            {
     			gl4dpSetColor(RGB(0,0,0));
      			gl4dpFilledCircle(padX+j*60+rect/2,padY+i*60+rect/2,22);          
     		    gl4dpSetColor(RGB(colorTable[pieceTable[j+i*8].color_piece].r,colorTable[pieceTable[j+i*8].color_piece].g,colorTable[pieceTable[j+i*8].color_piece].b));
            	gl4dpFilledCircle(padX+j*60+rect/2,padY+i*60+rect/2,15);          
   
            }
            else if(pieceTable[j+i*8].player==PLAYER2)
            {
     			gl4dpSetColor(RGB(255,255,255));
      			gl4dpFilledCircle(padX+j*60+rect/2,padY+i*60+rect/2,22);          
     		    gl4dpSetColor(RGB(colorTable[pieceTable[j+i*8].color_piece].r,colorTable[pieceTable[j+i*8].color_piece].g,colorTable[pieceTable[j+i*8].color_piece].b));
            	gl4dpFilledCircle(padX+j*60+rect/2,padY+i*60+rect/2,15);          
            } 
                   
	      }
    }


}

std::pair<int,double> kamisado::MCTS_selection(int courant, std::pair<int,double>max)
{
    if(arbre.find(courant)==arbre.end())
        return max;

    std::pair<int,double> maxAdj,maxTmp=max;
    mcts_node node=arbre.at(courant);

    if(max.second < node.win_play/(double)node.total_play)
    {
        maxTmp.first=courant;
        maxTmp.second=node.win_play/(double)node.total_play;
    }
    for(int i=0;i<node.adjacent.size();++i)
    {
        maxAdj=MCTS_selection(node.adjacent[i],maxTmp);  
        if(maxTmp.second<maxAdj.second)
            maxTmp=maxAdj;
    }
    return maxTmp;
}

void kamisado::MCTS_printArbre(int courant)
{
    if(arbre.find(courant)==arbre.end())
        return;
    mcts_node &n=arbre.at(courant);
    printf("ID: %d Parent: %d player: %d color: %d score:%d/%d\n",n.id,n.parent,n.player,n.ground_color,n.win_play,n.total_play);
    for(int i=0;i<WIDTH;i++)
    {
        for(int j=0;j<WIDTH;j++)
        {
            std::cout<<n.table[j+i*WIDTH].color_piece;
            if(n.table[j+i*WIDTH].player==PLAYER1)
                std::cout<<"A ";
            else if(n.table[j+i*WIDTH].player==PLAYER2)
               std::cout<<"B ";
            else
               std::cout<<"  ";
            
        }           
        std::cout<<"\n";
    }
    for(int i=0;i<n.adjacent.size();i++)
        MCTS_printArbre(n.adjacent[i]);


}

int kamisado::MCTS_expansion(int parent)
{
        mcts_node &node=arbre.at(parent); 
        mcts_node newNode;
        movement mv;
        if(node.player==PLAYER1)
        {
            mv = possible(node.table,PLAYER1,node.ground_color);
            newNode.player=PLAYER2;
        }
        else
        {
            mv = possible(node.table,PLAYER2,node.ground_color);
            newNode.player=PLAYER1;
        }
        for(int i=0;i<WIDTH*HEIGHT;i++)
        {
            newNode.table[i].x=node.table[i].x;
            newNode.table[i].y=node.table[i].y;
            newNode.table[i].player=node.table[i].player;
            newNode.table[i].color_piece=node.table[i].color_piece;
            newNode.table[i].color_ground=node.table[i].color_ground;
        }           
        node.adjacent.push_back(nodeId);        
        newNode.id=nodeId;
        newNode.parent=parent;
        newNode.win_play=1;
        newNode.total_play=1;
    //    std::cout<<mv.identifiants.size();
   //     std::cout<<mv.coordonnes.size();
        int randomNB2=(int)rand()%mv.identifiants.size();
        int randomNB=(int)rand()%mv.coordonnes[randomNB2].size();
        vertex ancien = {node.table[mv.identifiants[randomNB2]].x,node.table[mv.identifiants[randomNB2]].y};     
        newNode.ground_color=node.table[mv.coordonnes[randomNB2][randomNB].x+WIDTH*mv.coordonnes[randomNB2][randomNB].y].color_ground;
        placer(newNode.table,ancien,mv.coordonnes[randomNB2][randomNB]);
        newNode.move.first=ancien;        
        newNode.move.second=mv.coordonnes[randomNB2][randomNB];
        arbre[nodeId++]=newNode;

    return newNode.id;
}
void kamisado::MCTS_initialize(int player)
{
    arbre.clear();
    nodeId=0;
    mcts_node n;
    n.ground_color=lastGroundColor;
    n.id=nodeId;
    n.parent=0;
    n.win_play=1;
    n.player=player;
    n.total_play=1;
    for(int i=0;i<WIDTH*HEIGHT;i++)
    {
        n.table[i].x=pieceTable[i].x;
        n.table[i].y=pieceTable[i].y;
        n.table[i].player=pieceTable[i].player;
        n.table[i].color_piece=pieceTable[i].color_piece;
        n.table[i].color_ground=pieceTable[i].color_ground;
    }       
    arbre[nodeId++]=n;

}


int kamisado::MCTS_simulation(int id)
{
     mcts_node node=arbre.at(id);
     int lastColor=node.ground_color;
     int player=node.player;     
     int stop=1;
     while(stop!=0)
        {        
        movement mv;
        if(player==PLAYER1)        
        {
            mv = possible(node.table,PLAYER1,lastColor);
            player=PLAYER2;
        }
        else
        {
            mv = possible(node.table,PLAYER2,lastColor);
            player=PLAYER1;
        } 
   
//        assert(mv.identifiants.size()!=0);
        if(mv.identifiants.size()==0)
            return 0;
        int randomNB=rand()%mv.identifiants.size();    
        if(mv.coordonnes[randomNB].size()==0)
            return 0;
        int randomNB2=rand()%mv.coordonnes[randomNB].size();    
 
        vertex v;
        v.x=node.table[mv.identifiants[randomNB]].x;
        v.y=node.table[mv.identifiants[randomNB]].y;
        lastColor=node.table[mv.coordonnes[randomNB][randomNB2].x+WIDTH*mv.coordonnes[randomNB][randomNB2].y].color_ground;
        placer(node.table,v,mv.coordonnes[randomNB][randomNB2]);
    
        for(int i=0;i<WIDTH;i++)
        {
            if(node.table[i+WIDTH*START_POINT_PLAYER1].player==PLAYER2)
            {
                return PLAYER2;            
            }    
            if(node.table[i+WIDTH*START_POINT_PLAYER2].player==PLAYER1)
            {

                return PLAYER1;
            }    
        }
    }
    
}
void kamisado::MCTS_retroPropagation(int id,int score,int total)
{
    mcts_node & node = arbre.at(id);
    node.win_play+=score;
    node.total_play+=total;
    mcts_node nodeConst = arbre.at(id);
 
    while(1)
    {
        mcts_node & parent =arbre.at(nodeConst.parent);
        parent.win_play+=score;//nodeConst.win_play;
        parent.total_play+=total;//nodeConst.total_play;
        nodeConst=parent;
        if(nodeConst.parent==0)
            break;
    }

    mcts_node & root =arbre.at(0);
    root.win_play+=score;
    root.total_play+=total;
}

void kamisado::MCTS_AI(int player)
{
    std::pair<int,double> max;
    MCTS_initialize(player);

    for(int it_develop=0;it_develop<20;++it_develop)
    {
        max=MCTS_selection(0,(std::pair<int,double>){-1,-1.00});
        int expNode=MCTS_expansion(max.first);
        int score=0;    
        int total=20;
        for(int it_simulation=0;it_simulation<total;++it_simulation){
            
           if(MCTS_simulation(expNode)==player){
            score++;
            }
        }    
        MCTS_retroPropagation(expNode,score,total);
    }
   // MCTS_printArbre(0);


    mcts_node node=arbre.at(0);
    int maxIndex=-1;
    double maxVal=0;   
    for(int i=0;i<node.adjacent.size();i++)
    {
        mcts_node adj=arbre.at(node.adjacent[i]);        
        if(maxVal<adj.win_play/(double)adj.total_play)
        {
            maxIndex=node.adjacent[i];
            maxVal=adj.win_play/(double)adj.total_play;
        }   
    }
//    std::cout<<maxIndex;
    mcts_node probable=arbre.at(maxIndex);
    lastGroundColor=node.table[probable.move.second.x,probable.move.second.y].color_ground;
    placer(pieceTable,probable.move.first,probable.move.second);
    

 
}

void kamisado::playStep(void)
{
     if(etat==INIT)
     {
         if(turn==PLAYER1)
         {
             
             MCTS_AI(turn);
             turn=PLAYER2;            
         }
         else
         {                
                if(DFS_AI(PLAYER2)==-1){
                    std::cout << "match null\n";                
                    etat=END;
                }

             turn=PLAYER1;            
         }               
         gameover();
         if(PLAYER1==winner)
             std::cout << "winner MCTS\n";
         else if(PLAYER2==winner)
             std::cout << "winner DFS\n";
     }

}

/*
int main(){
    kamisado jeu;
    srand(time(NULL));
    //jeu.printColorTable();
    //jeu.printPieceTable();
//    jeu.play();
    jeu.MCTS_AI(PLAYER1);
    jeu.printColorTable();
    jeu.printPieceTable();
    
}

*/


