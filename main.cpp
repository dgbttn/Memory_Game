#include <iostream>
#include <cstdlib>
#include <ctime>

#include "Menu.h"
#include "Play.h"

using namespace std;

int main(int argc, char* argv[])
{
    srand(time(0));
    if (showMenu(1)) {
        if (Play()) {
            while (showMenu(0)) {
                if (Play()==0) break;
            }
        }
    }

    return 0;
}
