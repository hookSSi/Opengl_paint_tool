#include "GridMap.h"

bool GridMap::Init()
{
	Vertex *vertexList = new Vertex[(this->MAP_SIZE + 1) * (this->MAP_SIZE + 1)];

	for (int y = 0; y < MAP_SIZE; y++)
	{
		for (int x = 0; x < MAP_SIZE; x++)
		{
			Cube curCube = this->cubeList[y * MAP_SIZE + x];

			curCube.vetexList[0].Set(x / MAP_SIZE, y / MAP_SIZE);
			curCube.vetexList[1].Set(x / MAP_SIZE, (y + 1) / MAP_SIZE);
			curCube.vetexList[2].Set((x + 1) / MAP_SIZE, (y + 1) / MAP_SIZE);
			curCube.vetexList[3].Set((x + 1) / MAP_SIZE, 0);
		}
	}


	return true;
}