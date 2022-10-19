
#include "pch.h"

extern int main()
{
    try {
        return 0;
    }
    catch (int& code) {
        return code;
    }
}
