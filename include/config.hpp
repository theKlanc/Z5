#pragma once
class config{
	public:
		static inline int getChunkLoadRadius();
		static inline int getChunkSize();
		static inline int getRenderDistance();
	private:
	    config(){}
		static int chunkLoadRadius;
		static int chunkSize;
		static int renderDistance;
};