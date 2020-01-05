#include <iostream>
#include "scheduler/AtlasScheduler.h"

int main(int argc, char **argv)
{
    atlas::AtlasScheduler scheduler;

    scheduler.run();

    return 0;
};
