#include "utils.h"

int main(int argc, char **argv)
{
    print("Hello world from c...\n");
    while (1)
    {
        char c = getkey();
        if (c != 0)
        {
            print(c);
        }
    }
    return 0;
}