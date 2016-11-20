#ifndef __GRID_MAP_H_
#define __GRID_MAP_H_

// OpenGL 헤더파일
#include <GL/glut.h> 
// OpenGL 유틸리티
#include<gl/glu.h> 
#include<gl/glut.h>
// OpenGL 보조 함수들
#include<gl/glaux.h>
// 수학 관련 함수들
#include<math.h>


class Vertex
{
private:
	float x, y;
	float value;
public:
	Vertex(){ Vertex(0, 0); }
	Vertex(float _x, float _y) :x(_x), y(_y){ value = 0; }
	~Vertex(){}
	void Set(float _x, float _y){ this->x = _x; this->y = _y; }
	float GetX(){ return x; }
	float GetY(){ return y; }
	void SetValue(float _value){ value = _value; }
	float GetValue(){ return value; }
};

class Cube
{
public:
	Vertex *vetexList;

	Cube(){ vetexList = new Vertex[4]; }
	~Cube(){ delete[] this->vetexList; }
	void Draw();
};

class GridMap
{
private:
	int MAP_SIZE;

public:
	Cube *cubeList;

	GridMap(){ MAP_SIZE = 5; GridMap(MAP_SIZE); }
	GridMap(int _MAPSIZE):MAP_SIZE(_MAPSIZE){ cubeList = new Cube[MAP_SIZE, MAP_SIZE];}
	~GridMap(){ delete[] this->cubeList; }
	bool Init();
	void Draw();
};

#endif