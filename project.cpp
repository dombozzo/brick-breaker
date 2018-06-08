//Dominic Bozzo & Matthew J Malir
//Final Project (Lab 12), CSE 20311
//Block breaker
//12 December 2017

#include "gfxnew.h"
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>
using namespace std;

//make a struct to be the object of a block
struct block {
  int x;
  int y;
  int width;
  int height;
  int status;
};

//prototypes
void drawBlock(block);
vector <vector<block> > randomBlocks();
vector <vector<block> > pickConfig();
bool checkWin(vector <vector<block> >);

const int xsize = 501, ysize = 650;
const double pi = 3.1415926535897;
int main() {
  srand(time(NULL));
  char c=0;
  // Open a new window for drawing.
  gfx_open(xsize, ysize, "Brickbreaker");

  bool loop = true, playing = true, win = false, selected = false;
  double x , y, r , deltat;
  double vx, vy;
  int xpos,ypos;

  //create array of blocks and initialize them
  vector < vector<block> > blocks;


  //while loop for the menu
  while(playing){

    //Display menu options!
    gfx_text(92,200, "Welcome to Brickbreaker!");
    gfx_text(92,250, "Select Game Mode (click):");
    //draw rectangle and text for blocks
    gfx_rectangle(92,280,90,30);
    gfx_text(100,300, "Random Blocks");
    //draw rectangle and text for pre-loaded config
    gfx_rectangle(92,330,128,30);
    gfx_text(100,350,"Pre-defined Levels");
    //draw rectangle and text for quit
    gfx_rectangle(92,380,38,30);
    gfx_text(100,400,"Quit");

    while (!selected){
      c= gfx_wait();
      if (c == 1)
      {
        xpos = gfx_xpos();
        ypos = gfx_ypos();
        if (xpos >= 92 && xpos <= 182 && ypos >= 280 && ypos <= 310)
        {
          selected = true;
          blocks  = randomBlocks();
        }
        else if (xpos >= 92 && xpos <= 220 && ypos >= 330 && ypos <= 360)
        {
          selected = true;
          gfx_clear();
          blocks = pickConfig();
        }
        else if (xpos >= 92 && xpos <= 130 && ypos >= 380 && ypos <= 410)
        {
          selected = true;
          loop = false;
          playing = false;
        }
      }
    }
    selected = false; //reset flag

    gfx_clear();

    //define board
    block board;

    if (playing){
      //display board boundaries
      gfx_line(500,0,500,500);
      gfx_line(0,500,500,500);
      gfx_line(0,0,0,500);
      gfx_line(0,0,500,0);

      //display board at bottom to bounce ball off of
      board.height = 10;
      board.width = 50;
      board.y = xsize- 10 - board.height;
      board.x = (xsize/2) - (board.width / 2);
      gfx_fill_rectangle(board.x, board.y, board.width, board.height);

      //display initial Ball
      x = xsize/2;
      r = 5;
      y = xsize- r - 10 - board.height;
      gfx_circle(x,y,r);

      //display blocks
      for (int i = 0; i <8 ; i++){
        for (int j =0; j<10; j++){
          block b = blocks[i][j];
          if (b.status !=0){
            drawBlock(b);
          }
        }
      }

      //wait for user to select difficulty level
      gfx_text(130,300, "Enter a difficulty level to start (1,2,3)");
      while ((c!='1') && (c!='2') && (c!='3')){
        c= gfx_wait();
      }
    }//added in thing

    //set speeds according to difficultly level
    double angle = rand()%90 +45; //get angle to project ball at (btw 45 and 134)
    angle = angle * pi/ 180;
    double speed;
      //set velocities
      if (playing) {
        switch (c){
          case '1':
          vx = 1*cos(angle);
          vy = -1*sin(angle);
          speed =1;
          break;
          case '2':
          vx = 1.5*cos(angle);
          vy = -1.5*sin(angle);
          speed =1.5;
          break;
          case '3':
          vx = 2*cos(angle);
          vy = -2*sin(angle);
          speed=2;
          break;
        }
      };

      int distance;
      bool hit;
      int lives = 3;
      bool lifeLost = false;
      //loop to display ball moving
      while(loop){

        //move ball
        x = x + vx;
        y = y + vy;


        //check to see if it hits board at bottom
        if ((x >= board.x) && (x <= board.x + board.width) && (y+r >= board.y) && (y+r <= board.y+board.height)){
          //bounce off the board
          vy = -vy;
          y = 500-r-10 - board.height;; //force onto screen

          //figure out angle of tragectory. The further from center of board, the more severe the angle
          distance = x - (board.x + board.width/2);
          angle = (25 - distance) * 2 + 40;
          angle = angle *pi/180;
          vx = speed *cos(angle);
          vy = -speed *sin(angle);
        }

        //check to see if it hits edges of screen
        if (y <= r){
          //we've hit the top
          vy = -vy;
          //force the ball back onto screen
          y = r;
        }
        if (y>= (500 -r)){
          //we've hit bottom GAME OVER
          lives --;
          if (lives == 0){
            //game is over
            loop = false;
          }else{
            //reposition the original board and ball
            board.y = 500-10 - board.height;
            board.x = 500/2 - board.width / 2;
            x = 500/2;
            y = 500-r-10 - board.height;
            lifeLost = true;
          }

        }

        if ( x <= r ){
          //hit left
          vx = -vx;
          x = r; //force onto screen
        }
        if (x >= (500-r)){
          //hit right
          vx = -vx;
          x = 500 -r; //force onto screen
        }

        //check to see if blocks are hit
        hit = false;
        for (int i = 0; i <8 ; i++){
          for (int j =0; j<10; j++){
            block bl = blocks[i][j];
            if (bl.status !=0 && !hit){
              //ball hits bottom of brick
              if ((y-r <= bl.y+bl.height) && (y+r >= bl.y + bl.height) && (x>=bl.x) && (x<=bl.x+bl.width)){
                vy = -vy;
                y = bl.y + bl.height + r;
                blocks[i][j].status --;
                hit = true;
                break;
              }

              //ball hits right side of Brick
              else if ((x-r <= bl.x +bl.width) && (x+r >= bl.x +bl.width) && (y>=bl.y) && (y<= bl.y +bl.height)){
                vx = -vx;
                x = bl.x +bl.width +r;
                blocks[i][j].status --;
                hit = true;
                break;
              }

              // hits left side
              else if ((x+r >= bl.x) && (x-r <= bl.x) && (y>=bl.y) && (y<= bl.y +bl.height)){
                vx = -vx;
                x = bl.x -r;
                blocks[i][j].status --;
                hit = true;
                break;
              }

              //ball hits top of brick
              else if ((y+r >= bl.y) && (y-r <= bl.y) && (x>=bl.x) && (x<=bl.x+bl.width)){
                vy = -vy;
                y = bl.y  - r;
                blocks[i][j].status --;
                hit = true;
                break;
              }
            }

          }
          if (hit) break;
        }

        gfx_clear();
        //redraw board boundaries
        gfx_line(500,0,500,500);
        gfx_line(0,500,500,500);
        gfx_line(0,0,0,500);
        gfx_line(0,0,500,0);

        //redraw ball and board
        gfx_circle(x,y,r);
        gfx_fill_rectangle(board.x, board.y, board.width, board.height);

        //display in-game options
        gfx_rectangle(105,555,62,30);
        gfx_text(115,575, "p: pause");


        gfx_rectangle(216,555,78,30);
        gfx_text(225,575,"r: restart");


        gfx_rectangle(354,555,58,30);
        gfx_text(365,575,"q: quit");

        //write words
        switch (lives){
          case 3:
            gfx_text(225, 520, "Lives: 3");
            break;
          case 2:
            gfx_text(225, 520, "Lives: 2");
            break;
          case 1:
            gfx_text(225, 520, "Lives: 1");
            break;
        }

        gfx_flush();

        //display blocks if they are still alive
        for (int i = 0; i <8 ; i++){
          for (int j =0; j<10; j++){
            block b = blocks[i][j];
            if (b.status !=0){
              drawBlock(b);
            }
          }
        }
        gfx_flush();

        // if they lost a life on this iteration, then wait until they are ready to reset
        if (lifeLost){
          //hide the options because they are unavailable while game is not in play
          gfx_color(0,0,0);
          gfx_fill_rectangle(105,555,64,32);
          gfx_fill_rectangle(216,555,80,32);
          gfx_fill_rectangle(354,555,60,32);
          gfx_color(255,255,255);
          gfx_text(175,550,"Press space to continue");
          gfx_flush();
          //wait until they click space to start moving again
          while (c != ' '){
            c = gfx_wait();
          }

          angle = ((rand() % 90) + 45) * pi/180;
          //give new velocity
          if (speed == 1)
          {
            vx = 1*cos(angle);
            vy = -1*sin(angle);
          }
          else if (speed == 1.5)
          {
            vx = 1.5*cos(angle);
            vy = -1.5*sin(angle);
          }
          else if (speed == 2)
          {
            vx = 2*cos(angle);
            vy = -2*sin(angle);
          }
        }

        usleep(10000);

        //check to see if they won
        win = checkWin(blocks);
        if (win){
          //they won, so exit
          break;
        }

        if (gfx_event_waiting()){
          c = gfx_wait();
          switch (c){
            case 'Q':
            //move board to the left
            board.x = board.x-12;
            break;
            case 'S':
            //move board to the right
            board.x = board.x+12;
            break;
            case 'q': //quit
            loop = false;
            playing = false;
            break;
            case 'p': //pause
            {
              gfx_rectangle(105,595,135,30);
              //hide the quit and restart and pause options because they are unavailable
              gfx_color(0,0,0);
              gfx_fill_rectangle(105,555,64,32);
              gfx_fill_rectangle(216,555,80,32);
              gfx_fill_rectangle(354,555,60,32);
              gfx_color(255,255,255);

              gfx_text(115,615, "c: continue playing");
              gfx_flush();
              c = gfx_wait();
              while (c != 'c')
              {
                c = gfx_wait();
              }

              //redraw restart and quit options because they are available again

              break;
            }
            case 'r': //restart
              loop = false;
            break;
          }
        }

        lifeLost = false;
      } // while

    //the game is over
    if (win){
      gfx_text(230,250, "You won!");
      gfx_text(150,350, "Game will redirect to menu shortly");
      gfx_flush();
      usleep(3000000);
      gfx_clear();
      win = false; //reset for next game;
    } else {
      gfx_text(230,250, "Game Ended");
      gfx_flush();
      usleep(2000000);
      gfx_clear();
    }

    //reset variables
    c= ' ';
    loop = true;
  }//menu while


} //main

void drawBlock(block b){
  int stat = b.status;

  switch (stat){
    case 3:
      gfx_rectangle(b.x,b.y,b.width, b.height);
      gfx_line(b.x,b.y, b.x+b.width, b.y +b.height);
      gfx_line(b.x, b.y+b.height, b.x+b.width, b.y);
      break;
    case 2:
      gfx_rectangle(b.x,b.y,b.width, b.height);
      gfx_line(b.x,b.y, b.x+b.width, b.y +b.height);
      break;
    case 1:
      gfx_rectangle(b.x,b.y,b.width, b.height);
      break;
  }


}

//create randomized list of blocks
vector <vector<block> > randomBlocks(){
  block temp;
  int xplace=0, yplace =0;
  vector < vector<block> > blocks;
  vector<block>temp2;
  for (int i = 0; i <8 ; i++){
    for (int j =0; j<10; j++){
      temp.height = 25;
      temp.width = 50;
      temp.x = xplace;
      temp.y = yplace;
      temp.status = rand()%4;
      temp2.push_back(temp);
      xplace += 50;
    }
    blocks.push_back(temp2);
    temp2.clear();
    yplace +=25;
    xplace=0;
  }

  return blocks;
}
//create predetermined vector of blocks
vector <vector<block> > pickConfig(){
  bool selected = false;
  int choice;
  int xpos = 0,ypos = 0;
  char c;
  int cnt = 0;
  //custom display vectors
  vector<vector<int>> cs =
  {
    {0,0,0,0,0,0,0,0,0,0},
    {0,3,3,3,3,3,3,3,3,0},
    {0,3,0,0,0,3,0,0,0,0},
    {0,3,0,0,0,3,0,0,0,0},
    {0,3,0,0,0,3,3,3,3,0},
    {0,3,0,0,0,0,0,0,3,0},
    {0,3,0,0,0,0,0,0,3,0},
    {0,3,3,3,3,3,3,3,3,0}
  };
  vector<vector<int>> diamond =
  {
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,1,0,0,0,0},
    {0,0,0,2,2,2,2,0,0,0},
    {0,0,3,3,3,3,3,3,0,0},
    {0,3,3,3,3,3,3,3,3,0},
    {0,0,3,3,3,3,3,3,0,0},
    {0,0,0,2,2,2,2,0,0,0},
    {0,0,0,0,1,1,0,0,0,0}
  };
  vector<vector<int>> hut =
  {
    {0,0,0,1,1,1,1,0,0,0},
    {0,0,1,0,0,0,0,1,0,0},
    {0,1,0,0,2,2,0,0,1,0},
    {1,0,0,0,0,0,0,0,0,1},
    {1,0,2,0,0,0,0,2,0,1},
    {1,0,2,0,3,3,0,2,0,1},
    {1,0,0,0,3,3,0,0,0,1},
    {1,1,1,1,3,3,1,1,1,1}
  };
  //display choices to user
  //draw rectangle and text for CS
  gfx_rectangle(262,280,40,30);
  gfx_text(270,300, "CS");
  //draw rectangle and text for Diamond
  gfx_rectangle(262,330,56,30);
  gfx_text(270,350,"Diamond");
  //draw rectangle and text for Shrinking Squares
  gfx_rectangle(262,380,60,30);
  gfx_text(270,400,"Hut");

  while (!selected){
    c= gfx_wait();
    if (c == 1)
    {
      xpos = gfx_xpos();
      ypos = gfx_ypos();
      if (xpos >= 262 && xpos <= 302 && ypos >= 280 && ypos <= 310)
      {
        selected = true;
        choice = 1;
      }
      else if (xpos >= 262 && xpos <= 318 && ypos >= 330 && ypos <= 360)
      {
        selected = true;
        choice = 2;
      }
      else if (xpos >= 262 && xpos <= 322 && ypos >= 380 && ypos <= 410)
      {
        selected = true;
        choice = 3;
      }
    }
  }


  block temp;
  int xplace=0, yplace =0;
  vector < vector<block> > blocks;
  vector<block>temp2;
  for (int i = 0; i <8 ; i++){
    for (int j =0; j<10; j++){
      temp.height = 25;
      temp.width = 50;
      temp.x = xplace;
      temp.y = yplace;
      switch (choice)
      {
        case 1:
        {
          temp.status = cs[i][j];
          break;
        }
        case 2:
        {
          temp.status = diamond[i][j];
          break;
        }
        case 3:
        {
          temp.status = hut[i][j];
          break;
        }
      }
      temp2.push_back(temp);
      xplace += 50;
    }
    blocks.push_back(temp2);
    temp2.clear();
    yplace +=25;
    xplace=0;
  }

  return blocks;
}

bool checkWin(vector <vector<block> > blocks){
  //check to see if all blocks are dead
  bool flag = true;
  for (int i = 0; i <8 ; i++){
    for (int j =0; j<10; j++){
      if (blocks[i][j].status != 0) flag = false; //this means they didnt win yet
    }
  }
  return flag;
}
