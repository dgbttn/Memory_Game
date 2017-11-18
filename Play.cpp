#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include "Play.h"

using namespace std;

const int WIDTH= 1200;
const int HEIGHT= 680;
const string TITLE= "Memory Game!!";

#define fps 45
#define delay 1000/fps

enum
{
    EASY=0, MEDIUM, HARD
};

SDL_Window* Window;
SDL_Surface* Screen;
SDL_Renderer* Renderer;
TTF_Font *step_f, *score_f, *stage_f;
SDL_Texture* image[20], *ground, *bow, *ball, *col, *bar, *savescore, *savehscore, *savebscore;
SDL_Texture *youwin, *done, *done_s, *exit_button, *quitgame, *speaker[2];
SDL_Texture *stepw, *header, *stagew, *scorew;
point corner;
bool sound=1;

Mix_Music* beat[3];
Mix_Chunk *bonus, *choose, *click, *combo3, *combo4, *gameover, *match, *tiing, *wrong, *clap, *tenten;

a_card board[10][10];
bool check[10][10];
int m,n,sum, total_step, bonsum, step, stage, score;
int mode,diffi,topic,h_score[6];
string h_name[6];

SDL_Texture* loadTexture(string path)
{
    SDL_Texture* newTexture= NULL;
    SDL_Surface* loadedSurface= IMG_Load(path.c_str());
    if (loadedSurface==NULL) {cout<<"Error load image\n"; exit(1);}
    newTexture= SDL_CreateTextureFromSurface(Renderer, loadedSurface);
    SDL_FreeSurface(loadedSurface);
    return newTexture;
}

SDL_Texture* loadText(TTF_Font *font, string text, SDL_Color color)
{
    SDL_Texture* newTexture= NULL;
    SDL_Surface* textSurface= TTF_RenderText_Solid(font, text.c_str(), color);
    if (textSurface==NULL) {cout<<"Error font\n"; exit(1);}
    newTexture= SDL_CreateTextureFromSurface(Renderer, textSurface);
    SDL_FreeSurface(textSurface);
    return newTexture;
}

void loadFont()
{
    step_f= TTF_OpenFont("font/BERNHC.ttf", 58);
    stage_f= TTF_OpenFont("font/CURLZ.ttf", 80);
    score_f= TTF_OpenFont("font/BASKVILL.ttf", 50);
}

void loadImage()
{
    ground= loadTexture("pic/ground.PNG");
    bow= loadTexture("pic/bow.PNG");
    col= loadTexture("pic/col.PNG");
    ball= loadTexture("pic/ball.PNG");
    header= loadTexture("pic/header.PNG");
    savescore= loadTexture("pic/save_score.PNG");
    savehscore= loadTexture("pic/save_hscore.PNG");
    savebscore= loadTexture("pic/save_bscore.PNG");
    youwin= loadTexture("pic/youwin.PNG");
    done= loadTexture("pic/done.PNG");
    done_s= loadTexture("pic/done_s.PNG");
    exit_button= loadTexture("pic/exit.PNG");
    quitgame= loadTexture("pic/quitgame.PNG");

    speaker[0]= loadTexture("pic/Pmute.PNG");
    speaker[1]= loadTexture("pic/Pspeaker.PNG");

    image[0]= loadTexture("pic/00.PNG");
    image[1]= loadTexture("pic/01.PNG");
    image[2]= loadTexture("pic/02.PNG");
    image[3]= loadTexture("pic/03.PNG");
    image[4]= loadTexture("pic/04.PNG");
    image[5]= loadTexture("pic/05.PNG");
    image[6]= loadTexture("pic/06.PNG");
    image[7]= loadTexture("pic/07.PNG");
    image[8]= loadTexture("pic/08.PNG");
    image[9]= loadTexture("pic/09.PNG");
    image[10]= loadTexture("pic/10.PNG");
    image[11]= loadTexture("pic/11.PNG");
    image[12]= loadTexture("pic/12.PNG");
    image[13]= loadTexture("pic/13.PNG");
    image[14]= loadTexture("pic/14.PNG");
    image[15]= loadTexture("pic/15.PNG");
    image[16]= loadTexture("pic/16.PNG");
    image[17] = loadTexture("pic/+1.PNG");
    image[18] = loadTexture("pic/+2.PNG");
    image[19] = loadTexture("pic/+3.PNG");
}

void loadTrack()
{
    beat[0] = Mix_LoadMUS("play_soundtrack/play_beat_1.mp3");
    beat[1] = Mix_LoadMUS("play_soundtrack/play_beat_2.mp3");
    beat[2] = Mix_LoadMUS("play_soundtrack/play_beat_3.mp3");
    bonus = Mix_LoadWAV("play_soundtrack/tingting.wav");
    choose = Mix_LoadWAV("play_soundtrack/choose.mp3");
    click = Mix_LoadWAV("play_soundtrack/click.mp3");
    combo3 = Mix_LoadWAV("play_soundtrack/combo3.mp3");
    combo4 = Mix_LoadWAV("play_soundtrack/combo4.mp3");
    gameover = Mix_LoadWAV("play_soundtrack/game_over.mp3");
    match = Mix_LoadWAV("play_soundtrack/match.mp3");
    tiing = Mix_LoadWAV("play_soundtrack/tingting.wav");
    wrong = Mix_LoadWAV("play_soundtrack/wrong.mp3");
    clap = Mix_LoadWAV("play_soundtrack/clap.mp3");
    tenten = Mix_LoadWAV("play_soundtrack/tenten.mp3");
}

void initSDL()
{
    if (SDL_Init(SDL_INIT_EVERYTHING)==-1) exit(1);
    Window= SDL_CreateWindow(TITLE.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    Screen= SDL_GetWindowSurface(Window);

    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    Renderer= SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(Renderer, WIDTH, HEIGHT);
    SDL_SetRenderDrawColor(Renderer, 255,216, 81, 0x00);

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048)<0) exit(1);

    loadFont();
    loadImage();
    loadTrack();
}

void quitSDL()
{
    for (int i=0; i<=16; i++) SDL_DestroyTexture(image[i]);
    Mix_FreeChunk(tiing);
    SDL_DestroyTexture(ground);
    SDL_DestroyTexture(col);
    SDL_DestroyTexture(bow);
    SDL_DestroyTexture(ball);
    SDL_FreeSurface(Screen);
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);
    IMG_Quit();
    //Mix_Quit();
    //SDL_Quit();
}

void apply_surface(int x, int y, int w, int h, SDL_Texture* source)
{
    SDL_Rect offset;
    offset.x = x;
    offset.y = y;
    offset.w = w;
    offset.h = h;
    SDL_RenderCopy(Renderer, source, NULL, &offset);
}

void apply_col(int x, int y, int w, int h, int ts, int ms, SDL_Texture* source)
{
    SDL_Rect length;
    length.x = 0;
    length.y = 0;
    length.w = w;
    length.h = int ((ts*h)/ms);
    SDL_Rect offset;
    offset.x = x+8;
    offset.y = y+8+ (h-length.h);
    offset.w = w;
    offset.h = length.h;
    SDL_RenderCopy(Renderer, source, &length, &offset);
}

void chooseStage(int stage)
{
    bonsum=0;
    switch (stage) {
        case 1 : {m=2; n=2; break;}
        case 2 : {m=2; n=3; break;}
        case 3 : {m=3; n=3; bonsum=1; break;}
        case 4 : {m=4; n=3; bonsum=2; break;}
        case 5 : {m=4; n=4; bonsum=2; break;}
        case 6 : {m=5; n=4; bonsum=2; break;}
        case 7 : {m=6; n=5; bonsum=2; break;}
        case 8 : {m=7; n=6; bonsum=2; break;}
    }
    total_step=TTstep[diffi][stage] ;
}

void generateBoard()
{
    corner.y= (570 - (n*90-10))/2;
    corner.x= (1000 - (m*90-10))/2;
    step= total_step;

    int k=(m*n<9)? m*n/2 : (m*n-1)/2;
    int num=0,_i=0, _j=0;
    bool numed[17];
    sum=k*2;
    memset(check, false, sizeof(check));
    memset(numed, false, sizeof(numed));
    for (int i=0; i<=m; i++)
        for (int j=0; j<=n; j++)
            board[i][j].gotten= false;
    numed[num]= true;
    check[_i][_j]= true;

    for (int i=1; i<=min(k,16); i++)
    {
        while (numed[num]) num= rand()%16+1;
        numed[num]= true;
        while(check[_i][_j]){
            _i= rand()%m+1;
            _j= rand()%n+1;
        }
        check[_i][_j]= true;
        board[_i][_j].number= num;
        board[_i][_j].upface= 0;
        while(check[_i][_j]){
            _i= rand()%m+1;
            _j= rand()%n+1;
        }
        check[_i][_j]= true;
        board[_i][_j].number= num;
        board[_i][_j].upface= 0;
    }

    if (m*n<9) return;

    system("cls");

    while(check[_i][_j]){
        _i= rand()%m+1;
        _j= rand()%n+1;
    }
    check[_i][_j]= true;
    board[_i][_j].number= (m*n<=20)? 17 : 18;
    board[_i][_j].upface= 0;

    if (m*n==9) return;

    system("cls");

    while(check[_i][_j]){
        _i= rand()%m+1;
        _j= rand()%n+1;
    }
    check[_i][_j]= true;
    board[_i][_j].number= (m*n<=30)? 18 : 19;
    board[_i][_j].upface= 0;
    if (m*n<20) board[_i][_j].number=17;

    if (m*n<42) return;

    system("cls");
    for (int i=1; i<=4; i++)
    {
        while (!numed[num]) num= rand()%16+1;
        numed[num]= false;
        while(check[_i][_j]){
            _i= rand()%m+1;
            _j= rand()%n+1;
        }
        check[_i][_j]= true;
        board[_i][_j].number= num;
        board[_i][_j].upface= 0;
        while(check[_i][_j]){
            _i= rand()%m+1;
            _j= rand()%n+1;
        }
        check[_i][_j]= true;
        board[_i][_j].number= num;
        board[_i][_j].upface= 0;
    }

}

string numToStr(int k)
{
    string s="";
    if (k==0) return "0";
    char x;
    int i=0;
    while (k>0){
        x= (k%10)+48;
        s= x+s;
        i++;
        if (i==3) {
            if (k>9) s=','+s;
            i=0;
        }
        k/=10;
    }
    return s;
}

void showScore(int x, int y, SDL_Color color)
{
    int i=0;
    int k=1;
    while (score/k>0) {k*=10;i++;}
    if (score==0) i=1;
    scorew= loadText(score_f, numToStr(score), color);
    apply_surface(x-13*(i-1),y,24*i,51,scorew);

    apply_surface(650,570+55-40,80,80,exit_button);
    apply_surface(650+100,570+55-40,80,80,speaker[sound]);
}

void showStep()
{
    apply_surface(0,570,1200,110,header);
    apply_surface(1073,50,53,513,bow);
    apply_col(1073,50,37,500,step,total_step,col);
    apply_surface(1058,550,84,84,ball);
    stepw= loadText(step_f, numToStr(step), SDL_Color {255,0,0});
    if (step<10)
        apply_surface(1058+28,550+7,28,70,stepw);
    else
        apply_surface(1058+14,550+7,56,70,stepw);
    //SDL_RenderPresent(Renderer);
    stagew= loadText(stage_f, numToStr(stage), SDL_Color {0,0,255});
    apply_surface(195,590,23,67,stagew);
    showScore(426,613, SDL_Color {255,255,255});
}

void TransStep(int up)
{
    SDL_Rect length;
    length.x = 0;
    length.y = 0;
    length.w = 37;
    SDL_Rect offset;
    offset.x = 1073+8;
    offset.w = 37;

    //SDL_RenderCopy(Renderer, source, &length, &offset);

    int start= (int) ((step-up)*500)/total_step;
    int finish= (int) (step*500)/total_step;
    int go= start;

    if (up>0){
        while(go+5<finish){
            apply_surface(1073,50,53,514,bow);

            go= min(go+5,finish);
            length.h= go;
            offset.y = 50+8+ (500-length.h);
            offset.h = length.h;
            SDL_RenderCopy(Renderer, col, &length, &offset);

            apply_surface(1058,550,84,84,ball);
            stepw= loadText(step_f, numToStr(step), SDL_Color {255,0,0});
            if (step<10)
                apply_surface(1058+28,550+7,28,70,stepw);
            else
                apply_surface(1058+14,550+7,56,70,stepw);
            SDL_RenderPresent(Renderer);
            SDL_Delay(10);
        }
        showStep();
        SDL_RenderPresent(Renderer);
        return;
    }
    while(go-5>finish){
        apply_surface(1073,50,53,514,bow);

        go= max(go-5,finish);
        length.h= go;
        offset.y = 50+8+ (500-length.h);
        offset.h = length.h;
        SDL_RenderCopy(Renderer, col, &length, &offset);

        apply_surface(1058,550,84,84,ball);
        stepw= loadText(step_f, numToStr(step), SDL_Color {255,0,0});
        if (step<10)
            apply_surface(1058+28,550+7,28,70,stepw);
        else
            apply_surface(1058+14,550+7,56,70,stepw);
        SDL_RenderPresent(Renderer);
        SDL_Delay(10);
    }
    showStep();
    SDL_RenderPresent(Renderer);
}

void drawBoard()
{
    //SDL_RenderClear(Renderer);
    apply_surface(0,0,WIDTH,HEIGHT,ground);

    //apply_surface(20,515,)
    for (int i=1; i<=m; i++)
        for (int j=1; j<=n; j++){
            if (board[i][j].gotten) continue;
            if (board[i][j].upface)
                apply_surface(corner.x+(i-1)*90, corner.y+(j-1)*90, 80, 80, image[board[i][j].number]);
            else
                apply_surface(corner.x+(i-1)*90, corner.y+(j-1)*90, 80, 80, image[0]);
        }

    showStep();

    //SDL_RenderPresent(Renderer);
}

void introBoard()
{
    apply_surface(0,0,1200,680,ground);
    SDL_RenderPresent(Renderer);
    SDL_Delay(100);
    for (int j=0; j<2; j++){
        for (int i=0; i<=7; i++){
            apply_surface(0,0,1200,680,ground);
            apply_surface(1058+(7-i)*6, 300,i*12 ,84, ball);
            SDL_RenderPresent(Renderer);
            SDL_Delay(24);
        }
        for (int i=7; i>=0; i--){
            apply_surface(0,0,1200,680,ground);
            apply_surface(1058+(7-i)*6, 300,i*12 ,84, ball);
            SDL_RenderPresent(Renderer);
            SDL_Delay(24);
        }
    }
    for (int i=0; i<=7; i++){
        apply_surface(0,0,1200,680,ground);
        apply_surface(1058+(7-i)*6, 300,i*12 ,84, ball);
        SDL_RenderPresent(Renderer);
        SDL_Delay(24);
    }

    for (int i=0; i<=600; i++){
        apply_surface(0,0,1200,680,ground);
        apply_surface(i*2-1200,570,1200,110,header);
        apply_col(1073-8,250+50-8-500+min(i,250), 53, 513,2*min(i,250),500,bow);
        apply_surface(1058,300+min(i,250),84,84,ball);
        SDL_RenderPresent(Renderer);
        SDL_Delay(4);
    }
}

point pos(int xx, int yy)
{
    point res;
    res.x= -1; res.y= -1;
    int i=1, j=1;
    while (i<=m && (xx<corner.x+(i-1)*90 || xx>corner.x+(i-1)*90+80)) i++;
    if (i>m) return res;;
    while (j<=n && (yy<corner.y+(j-1)*90 || yy>corner.y+(j-1)*90+80)) j++;
    if (j>n) return res;
    res.x= i; res.y= j;
    return res;
}

void Trans1card(SDL_Texture* T, int xx, int yy)
{
    point p;
    p.x= corner.x + (xx-1)*90;
    p.y= corner.y + (yy-1)*90;
    board[xx][yy].gotten= true;
    for (int i=0; i<=5; i++) {
        drawBoard();
        apply_surface(p.x+i*8,p.y,80-i*16,80,image[0]);
        SDL_RenderPresent(Renderer);
        SDL_Delay(delay-2);
    }
    for (int i=5; i>=0; i--) {
        drawBoard();
        apply_surface(p.x+i*8,p.y,80-i*16,80,T);
        SDL_RenderPresent(Renderer);
        SDL_Delay(delay-2);
    }
    board[xx][yy].upface= 1;
    board[xx][yy].gotten= false;
}

void swap2cards(SDL_Texture* T1, int i1, int j1, SDL_Texture* T2, int i2, int j2)
{
    point p1, p2;
    p1.x= corner.x+(i1-1)*90;
    p1.y= corner.y+(j1-1)*90;
    p2.x= corner.x+(i2-1)*90;
    p2.y= corner.y+(j2-1)*90;
    board[i1][j1].gotten= true;
    board[i2][j2].gotten= true;
    if (abs(p1.x-p2.x)>=abs(p1.y-p2.y)) {
        for (int i=0; i<=abs(p1.x-p2.x); i++) {
            drawBoard();
            apply_surface(p1.x-i*(p1.x-p2.x)/abs(p1.x-p2.x), p1.y-(int) (i*(p1.y-p2.y)/abs(p1.x-p2.x)),80,80,T1);
            apply_surface(p2.x+i*(p1.x-p2.x)/abs(p1.x-p2.x), p2.y+(int) (i*(p1.y-p2.y)/abs(p1.x-p2.x)),80,80,T2);
            SDL_RenderPresent(Renderer);
            SDL_Delay(3);
        }
    }
    else {
        for (int i=0; i<=abs(p1.y-p2.y); i++) {
            drawBoard();
            apply_surface(p1.x-(int) (i*(p1.x-p2.x)/abs(p1.y-p2.y)), p1.y-i*(p1.y-p2.y)/abs(p1.y-p2.y),80,80,T1);
            apply_surface(p2.x+(int) (i*(p1.x-p2.x)/abs(p1.y-p2.y)), p2.y+i*(p1.y-p2.y)/abs(p1.y-p2.y),80,80,T2);
            SDL_RenderPresent(Renderer);
            SDL_Delay(3);
        }
    }
    a_card tmp= board[i1][j1];
    board[i1][j1] = board[i2][j2];
    board[i2][j2]= tmp;
    board[i1][j1].gotten= false;
    board[i2][j2].gotten= false;
}

void Trans2cards(SDL_Texture* T1, int i1, int j1, SDL_Texture* T2, int i2, int j2)
{
    point p1, p2;
    p1.x= corner.x+(i1-1)*90;
    p1.y= corner.y+(j1-1)*90;
    p2.x= corner.x+(i2-1)*90;
    p2.y= corner.y+(j2-1)*90;
    board[i1][j1].gotten= true;
    board[i2][j2].gotten= true;
    for (int i=0; i<=5; i++) {
        drawBoard();
        apply_surface(p1.x+i*8,p1.y,80-i*16,80,T1);
        apply_surface(p2.x+i*8,p2.y,80-i*16,80,T2);
        SDL_RenderPresent(Renderer);
        SDL_Delay(delay-2);
    }
    for (int i=5; i>=0; i--) {
        drawBoard();
        apply_surface(p1.x+i*8,p1.y,80-i*16,80,image[0]);
        apply_surface(p2.x+i*8,p2.y,80-i*16,80,image[0]);
        SDL_RenderPresent(Renderer);
        SDL_Delay(delay-2);
    }
    board[i1][j1].upface= 0;
    board[i2][j2].upface= 0;
    board[i1][j1].gotten= false;
    board[i2][j2].gotten= false;
}

void DisappBonus(SDL_Texture* T1, int i1, int j1)
{
    point p1;
    p1.x= corner.x+(i1-1)*90;
    p1.y= corner.y+(j1-1)*90;
    board[i1][j1].gotten= true;
    for (int i=0; i<=5; i++) {
        drawBoard();
        apply_surface(p1.x+i*8,p1.y+i*8,80-i*16,80-i*16,T1);
        SDL_RenderPresent(Renderer);
        SDL_Delay(delay-2);
    }
    board[i1][j1].upface= 0;
}

void Disapp2cards(SDL_Texture* T1, int i1, int j1, SDL_Texture* T2, int i2, int j2)
{
    point p1, p2;
    p1.x= corner.x+(i1-1)*90;
    p1.y= corner.y+(j1-1)*90;
    p2.x= corner.x+(i2-1)*90;
    p2.y= corner.y+(j2-1)*90;
    board[i1][j1].gotten= true;
    board[i2][j2].gotten= true;
    for (int i=0; i<=5; i++) {
        drawBoard();
        apply_surface(p1.x+i*8,p1.y+i*8,80-i*16,80-i*16,T1);
        apply_surface(p2.x+i*8,p2.y+i*8,80-i*16,80-i*16,T2);
        SDL_RenderPresent(Renderer);
        SDL_Delay(delay-2);
    }
    board[i1][j1].upface= 0;
    board[i2][j2].upface= 0;
    sum-=2;
}

int TransAllBoard()
{
    int xx,yy,dem=0;
    point p;

    for (int i=0; i<=m; i++)
        for (int j=0; j<=n; j++)
            if (!board[i][j].gotten&&board[i][j].number>=16&&board[i][j].number<=18) dem++;
    for (int i=0; i<=5; i++) {
        apply_surface(0,0,WIDTH,HEIGHT,ground);
        for (xx=1; xx<=m; xx++)
            for (yy=1; yy<=n; yy++ )
                if (!board[xx][yy].gotten) {
                    p.x= corner.x+(xx-1)*90;
                    p.y= corner.y+(yy-1)*90;
                    if (board[xx][yy].upface) {apply_surface(p.x,p.y,80,80,image[board[xx][yy].number]); continue;}
                    apply_surface(p.x+i*8,p.y,80-i*16,80,image[0]);
                }
        showStep();
        SDL_RenderPresent(Renderer);
        SDL_Delay(delay-2);
    }
    for (int i=5; i>=0; i--) {
        apply_surface(0,0,WIDTH,HEIGHT,ground);
        for (xx=1; xx<=m; xx++)
            for (yy=1; yy<=n; yy++ )
                if (!board[xx][yy].gotten) {
                    p.x= corner.x+(xx-1)*90;
                    p.y= corner.y+(yy-1)*90;
                    if (board[xx][yy].upface) {apply_surface(p.x,p.y,80,80,image[board[xx][yy].number]); continue;}
                    apply_surface(p.x+i*8,p.y,80-i*16,80,image[board[xx][yy].number]);
                }
        showStep();
        SDL_RenderPresent(Renderer);
        SDL_Delay(delay-2);
    }
    return dem;
}

string del1(string s)
{
    string res= "";
    for (int i=0; i<max(0,(int) (s.length()-1)); i++) res= res+s[i];
    return res;
}

string inputName(int Rank)
{
    Uint32 time, start;
    string name= "";
    string cs="|";
    SDL_Texture* csw= loadText(step_f,cs,SDL_Color {0,0,255});
    SDL_Texture* namew;
    int len=0;
    bool up= true;
    const int Max_len=15;
    SDL_Event e;
    start= SDL_GetTicks();
    if (stage>8||Rank==0) Mix_PlayChannel( -1, clap, 0 );
        else Mix_PlayChannel( -1, tenten, 0 );

    while (true) {
        if (Rank<5) apply_surface(333,186,533,308,savehscore);
        if (Rank==0) apply_surface(333,186,533,308,savebscore);
        if (Rank==5) apply_surface(333,186,533,308,savescore);
        if (stage>8||Rank==0) apply_surface(770,0,337,260,youwin);
        apply_surface(333+533/2-215/2,186+308+10,215,83,done);
        showScore(265+333,100+186,SDL_Color {255,0,0});
        if (SDL_PollEvent(&e)!=0) {
            if (e.type==SDL_KEYDOWN){
                switch (e.key.keysym.sym) {
                    case SDLK_BACKSPACE : {
                        if (name.length()==0) break;
                        if (name[name.length()-1]==' ') len-= 14;
                        else len-= 28;
                        name= del1(name);
                        break;
                    }
                    case SDLK_SPACE : {
                        if (name.length()==Max_len) break;
                        name= name+' ';
                        len+= 14;
                        break;
                    }
                    case SDLK_RETURN : {
                        if (name.length()>0){
                            namew= loadText(step_f,name,SDL_Color {0,0,255});
                            apply_surface(50+333,186+227,len,60,namew);
                            if (up) apply_surface(50+333+len,186+220,20,70,csw);
                            apply_surface(333+533/2-215/2,186+308+10,215,83,done_s);
                            SDL_RenderPresent(Renderer);
                            SDL_Delay(300);
                            return name;
                        }
                        break;
                    }
                }
                if (name.length()<Max_len) {
                    if (e.key.keysym.sym>=48 && e.key.keysym.sym<=57) {
                        len+=28;
                        name= name+ (char) (e.key.keysym.sym);
                    }
                    if (e.key.keysym.sym>=97 && e.key.keysym.sym<=122) {
                        len+=28;
                        name= name+ (char) (e.key.keysym.sym-32);
                    }
                }
            }
            if (e.type==SDL_MOUSEBUTTONDOWN) {
                if (e.button.x>=492&&e.button.x<=707&&e.button.y>=504&&e.button.y<=587&&name.length()>0)
                {
                    namew= loadText(step_f,name,SDL_Color {0,0,255});
                    apply_surface(50+333,186+227,len,60,namew);
                    if (up) apply_surface(50+333+len,186+220,20,70,csw);
                    apply_surface(333+533/2-215/2,186+308+10,215,83,done_s);
                    SDL_RenderPresent(Renderer);
                    SDL_Delay(300);
                    return name;
                }
            }
        }
        if (name.length()>0){
            namew= loadText(step_f,name,SDL_Color {0,0,255});
            apply_surface(50+333,186+227,len,60,namew);
        }
        time= SDL_GetTicks()-start;
        if (time>delay*25) {
            up=!up;
            start= SDL_GetTicks();
        }
        if (up) apply_surface(50+333+len,186+220,20,70,csw);
        SDL_RenderPresent(Renderer);
        SDL_Delay(15);
    }

}

void gameOver(bool playing)
{
    Mix_HaltMusic();
    if (stage<=8&&playing) {
        Mix_PlayChannel( -1, gameover, 0 );
        TransAllBoard();
        SDL_Delay(4100);
    }

    ifstream fi;
    switch (diffi) {
        case 0: {fi.open("high_score_0.txt",ios::in); break;}
        case 1: {fi.open("high_score_1.txt",ios::in); break;}
        case 2: {fi.open("high_score_2.txt",ios::in); break;}
    }

    for (int i=0; i<5; i++){
        getline(fi,h_name[i]);
        fi>>h_score[i];
    }

    int Rank=5;
    while (Rank>0&&score>h_score[Rank-1]) {
        h_score[Rank]= h_score[Rank-1];
        h_name[Rank]= h_name[Rank-1];
        h_score[Rank-1]= score;
        Rank--;
    }
    h_name[Rank]= ' '+inputName(Rank);

    ofstream fo;

    switch (diffi) {
        case 0: {fo.open("high_score_0.txt",ios::out); break;}
        case 1: {fo.open("high_score_1.txt",ios::out); break;}
        case 2: {fo.open("high_score_2.txt",ios::out); break;}
    }
    for (int i=0; i<5; i++)
        fo<<h_name[i]<<endl<<h_score[i];
}

void nextStage()
{
    int k;
    if (sum||bonsum) k=TransAllBoard();
    else k=0;
    step+=k;
    total_step= max(step, total_step);
    if (k) {
        Mix_PlayChannel( -1, bonus, 0 );
        TransStep(k);
    }

    while (step){
        Mix_PlayChannel( -1, tiing, 0 );
        step--;
        TransStep(-1);
        score+=100;
    }
    stage++;
}

void getGameMode()
{
    ifstream fi("game_mode.txt");
    fi>>diffi>>topic;
}

bool quitGame()
{
    apply_surface(0,0,1200,680,quitgame);
    SDL_RenderPresent(Renderer);
    SDL_Event e;
    while (true) {
        if (SDL_WaitEvent(&e)&&e.type==SDL_MOUSEBUTTONDOWN) {
            if (e.button.x>=376+73&&e.button.y>=212+178&&e.button.x<=376+73+140&&e.button.y<=212+178+75) {
                Mix_PlayChannel( -1, click, 0 );
                return 1;
            }
            if (e.button.x>=376+239&&e.button.y>=212+177&&e.button.x<=376+239+140&&e.button.y<=212+177+61) {
                Mix_PlayChannel( -1, click, 0 );
                return 0;
            }
        }
        SDL_Delay(delay);
    }
}

void tBonus(int i, int j)
{
    SDL_Delay(delay*15);
    DisappBonus(image[board[i][j].number],i,j);
    bonsum--;
    step+= board[i][j].number-16;
    if (total_step<step) {
        total_step= max(step, total_step);
        showStep();
        SDL_RenderPresent(Renderer);
    }
    else {
        total_step= max(step, total_step);
        TransStep(board[i][j].number-16);
    }
}

void wrongChoice(int i1, int j1, int i2, int j2)
{
    if (diffi==HARD){
        swap2cards(image[board[i1][j1].number], i1 , j1,
                   image[board[i2][j2].number], i2 , j2);
        SDL_Delay(400);
    }

    Trans2cards(image[board[i1][j1].number], i1 , j1,
                image[board[i2][j2].number], i2 , j2);
}

void matchChoice(int i1, int j1, int i2, int j2)
{
    score+=100;

    Disapp2cards(image[board[i1][j1].number], i1 , j1,
                image[board[i2][j2].number], i2 , j2);
}

void Transound()
{
    Mix_PlayChannel( -1, click, 0 );

    if (Mix_PausedMusic()) Mix_ResumeMusic();
    else Mix_PauseMusic();

    sound= !sound;
    apply_surface(650+100,570+55-40,80,80,speaker[sound]);
    SDL_RenderPresent(Renderer);
}

bool Play()
{
    initSDL();
    getGameMode();
    int i,j,combo=0;
    stage=1;
    score=0;
    SDL_Event e;
    introBoard();
    Mix_PlayMusic(beat[rand()%3],-1);
    if (!sound) Mix_PauseMusic();
    Uint32 time, start;
    while (SDL_PollEvent(&e)) {}
    while (stage<=8){
        chooseStage(stage);
        generateBoard();
        drawBoard();
        SDL_RenderPresent(Renderer);

        point p[2];
        int cnt=0;

        while (true){
            if (SDL_WaitEvent(&e)==0) continue;
            if (e.type==SDL_QUIT){
                SDL_Quit();
                return 0;
            }
            if (e.type==SDL_MOUSEBUTTONDOWN){
                p[cnt]= pos(e.button.x, e.button.y);
                if (e.button.x>=650&&e.button.y>=585&&e.button.x<=650+80&&e.button.y<=585+80) { //pause_button
                    Mix_PlayChannel( -1, click, 0 );
                    if (quitGame()) {
                        gameOver(0);
                        break;
                    }
                    else {
                        drawBoard();
                        SDL_RenderPresent(Renderer);
                    }
                }

                if (e.button.x>=750&&e.button.y>=585&&e.button.x<=750+80&&e.button.y<=585+80) { //mute button
                    Transound();
                }

                if (p[cnt].x==-1) continue; //click outside

                i= p[cnt].x; j= p[cnt].y;
                if (board[i][j].gotten || board[i][j].upface) continue; //wrong click

                if (!cnt) Mix_PlayChannel( -1, choose, 0 );
                Trans1card(image[board[i][j].number], i, j);

                if (board[i][j].number>=17&&board[i][j].number<=19) {
                    Mix_PlayChannel( -1, bonus, 0 );
                    tBonus(i,j);
                    continue;
                }
                cnt++;

                if (cnt==2) {
                    step--;
                    start= SDL_GetTicks();
                    TransStep(-1);
                    if (board[p[1].x][p[1].y].number != board[p[0].x][p[0].y].number){
                        Mix_PlayChannel( -1, wrong, 0 );
                        if (step==0&&sum>0) {
                            gameOver(1);
                            quitSDL();
                            return 1;
                        }
                        time= SDL_GetTicks() - start;
                        SDL_Delay(max(0,(int) (delay*30-time)));

                        wrongChoice(p[0].x,p[0].y,p[1].x,p[1].y);
                        combo=0;
                    }
                    else{
                        Mix_PlayChannel( -1, match, 0 );
                        time= SDL_GetTicks() - start;
                        SDL_Delay(max(0,(int) (delay*17-time)));

                        matchChoice(p[0].x,p[0].y,p[1].x,p[1].y);
                        combo++;

                        if (combo==3) Mix_PlayChannel( -1, combo3, 0 );
                        if (combo==4) Mix_PlayChannel( -1, combo4, 0 );

                        if (step==0&&sum>0) {
                            gameOver(1);
                            quitSDL();
                            return 1;
                        }
                    }
                    cnt=0;
                }
                if (sum==0) break;
            }
            SDL_Delay(delay);
        }
        if (sum>0) break; // chi khi KEYDOWN
        //if (bonsum) gameOver(); // lat het card thanh cong
        nextStage();
        SDL_Delay(delay*5);
    }
    if (stage>8) gameOver(0);
    quitSDL();
    return 1;
}
