#pragma once

#include <iostream>
#include <cstring>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

using namespace std;

const int TTstep[3][9]= {{0,4,7,9,12,16,22,33,47},{0,3,5,6,8,12,16,25,37},{0,4,7,9,12,16,22,33,47}};

struct a_card
{
    int number;
    bool upface=0, gotten=0;
};

struct point
{
    int x,y;
};

bool Play();
