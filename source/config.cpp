#include "config.hpp"

int config::getChunkLoadRadius(){return chunkLoadRadius;}
int config::getChunkSize(){return chunkSize;};
int config::getRenderDistance(){return renderDistance;};

int config::chunkLoadRadius=8;
int config::chunkSize=32;
int config::renderDistance=4;
