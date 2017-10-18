// A_Star.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
//#include <map> // map
#include <vector>
#include <stdlib.h>
//#include <atltypes.h> // CPoint
#include <winsock.h>
#include <conio.h> // _getch()
#include <time.h>
#include <math.h>

#define MAX_Y 10 // Max liczba wierszy
#define MAX_X 15 // Max liczba kolumn

using namespace std;

// Unused
/*struct Punkt`
{
	int X;
	int Y;
};
map<Punkt, Pole> mapOpenList;
map<Punkt, Pole> mapClosedList;
map<Punkt, Pole> mapObstacleList;
map<int,int, Pole> mapTest;*/

struct Pole
{
	int X;
	int Y;
};


void RenderGrid();
void RenderGrid(Pole cameFrom[MAX_Y][MAX_X], Pole current);

vector<Pole> vecOpenList;
vector<Pole> vecClosedList;
vector<Pole> vecObstacleList;

Pole start;
Pole finish;

bool IsObstacle(Pole pole)
{
	for (int i = 0; i<vecObstacleList.size(); i++)
		if (vecObstacleList[i].X == pole.X && vecObstacleList[i].Y == pole.Y)
			return true;
	return false;
}

int HeuristicCostEstimate(Pole pole)
{
	int nCost = abs(finish.X - pole.X);
	nCost += abs(finish.Y - pole.Y);
	return nCost;
}

bool IsGoal(Pole pole)
{
	if (pole.X == finish.X && pole.Y == finish.Y)
		return true;
	return false;
}

Pole GetBestCandidate(int fScore[MAX_Y][MAX_X])
{
	Pole pole = vecOpenList[0];
	int nCurfScore = fScore[pole.Y][pole.X];
	
	for(int i =1;i<vecOpenList.size();i++ )
	{
		if(fScore[vecOpenList[i].Y][vecOpenList[i].X] < nCurfScore)
		{
			pole = vecOpenList[i];
			nCurfScore = fScore[vecOpenList[i].Y][vecOpenList[i].X];
		}
	}
	return pole;
}

void RemoveFromOpenList(Pole pole)
{
	for( vector<Pole>::iterator it = vecOpenList.begin(); it != vecOpenList.end(); ++it)
	{
		if(pole.X == it->X && pole.Y == it->Y)
		{
			vecOpenList.erase(it);
			break;
		}
	}
}

bool IsInOpenList(Pole pole)
{
	for( vector<Pole>::iterator it = vecOpenList.begin(); it != vecOpenList.end(); ++it)
		if(pole.X == it->X && pole.Y == it->Y)
			return true;
	return false;
}

bool IsInClosedList(Pole pole)
{
	for( vector<Pole>::iterator it = vecClosedList.begin(); it != vecClosedList.end(); ++it)
		if(pole.X == it->X && pole.Y == it->Y)
			return true;
	return false;
}

vector<Pole> GetNeighbours(Pole pole)
{
	//printf("Making Neighbours:\n");
	Pole neighbour;
	vector<Pole> vecNeighbours;

	for( int i=0; i< 4; i++)
	{
		switch(i)
		{
		case 0: // Lewo
			neighbour.X = pole.X-1;neighbour.Y = pole.Y; break;
		case 1: // Prawo
			neighbour.X = pole.X+1;neighbour.Y = pole.Y; break;
		case 2: // Góra
			neighbour.X = pole.X;neighbour.Y = pole.Y-1; break;
		case 3: // Dół
			neighbour.X = pole.X;neighbour.Y = pole.Y+1; break;
		}

		if( neighbour.X < 0 || neighbour.X > MAX_X-1 || neighbour.Y < 0 || neighbour.Y > MAX_Y-1)
			continue;

		if( IsObstacle(neighbour))
			continue;

		if( IsInClosedList(neighbour))
			continue;
		//printf("Cords: %d %d\n", neighbour.X, neighbour.Y);
		vecNeighbours.push_back(neighbour);
	}

	return vecNeighbours;
}

int DistBetween(Pole current, Pole neighbour)
{
	return abs(current.X-neighbour.X)+abs(current.Y-neighbour.Y);
}

void AStar()
{
	vecClosedList.clear();
	vecOpenList.clear();
	vecOpenList.push_back(start);

	Pole cameFrom[MAX_Y][MAX_X];

	int gScore[MAX_Y][MAX_X];
	int fScore[MAX_Y][MAX_X];

	for(int i=0;i<MAX_Y;i++)
		for(int k=0;k<MAX_X;k++)
		{
			gScore[i][k] = LONG_MAX;
			fScore[i][k] = LONG_MAX;
		}

	gScore[start.Y][start.X] = 0;

	fScore[start.Y][start.X] = HeuristicCostEstimate(start);

	while(vecOpenList.size() > 0)
	{
		Pole current = GetBestCandidate(fScore);
		if( IsGoal(current))
		{
			RenderGrid(cameFrom, current);

			goto END;
			//return;
		}
		// Show each step
		RenderGrid(cameFrom, current);
		
		RemoveFromOpenList(current);
		vecClosedList.push_back(current);

		vector<Pole> vecNeighbours = GetNeighbours(current);

		for(int i=0;i<vecNeighbours.size();i++)
		{
			Pole neighbour = vecNeighbours[i];
			if( !IsInOpenList(neighbour))
				vecOpenList.push_back(neighbour);

			int tentative_gScore = gScore[current.Y][current.X] + DistBetween(current,neighbour);
			if( tentative_gScore >= gScore[neighbour.Y][neighbour.X])
				continue;

			cameFrom[neighbour.Y][neighbour.X] = current;
			gScore[neighbour.Y][neighbour.X] = tentative_gScore;
			fScore[neighbour.Y][neighbour.X] = tentative_gScore + HeuristicCostEstimate(neighbour);
		}
	}


	printf("\n\n NIE ZNALEZIONO DROGI!!!\n\n\n");

END:
	// gScore Table debug
	for(int i=0;i<MAX_Y;i++)
	{
		for(int k=0;k<MAX_X;k++)
			printf("%10d ", gScore[i][k]);
		printf("\n");
	}
	printf("\n");
	// fScore Table debug
	for(int i=0;i<MAX_Y;i++)
	{
		for(int k=0;k<MAX_X;k++)
			printf("%d ", fScore[i][k]);
		printf("\n");
	}
}


void GenerateObstacles()
{
	vecObstacleList.clear();
	/*Pole pole;
	pole.X = 3;
	pole.Y = 1;
	vecObstacleList.push_back(pole);
	pole.Y = 2;
	vecObstacleList.push_back(pole);
	pole.Y = 3;
	vecObstacleList.push_back(pole);
	pole.X = 4;
	pole.Y = 1;
	vecObstacleList.push_back(pole);
	*/

	int nObstacles = MAX_Y*MAX_X / 3;

	while (nObstacles > 0)
	{
		Pole pole;
		pole.X = rand() % MAX_X;
		pole.Y = rand() % MAX_Y;
		if (IsObstacle(pole))
			continue;
		if (start.X == pole.X && start.Y == pole.Y || IsGoal(pole))
			continue;
		vecObstacleList.push_back(pole);
		nObstacles--;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	srand(time(NULL));
	// Start coordinates
	start.X = 1;
	start.Y = 2;
	// End coordinates
	finish.X = 14;
	finish.Y = 9;
	//finish.X = 4;
	//finish.Y = 2;

	char chKey;
	while(chKey = _getch())
	{
		chKey = toupper(chKey);

		if( chKey == 'Q')
			break;

		if( chKey == 'A')
			RenderGrid();

		if (chKey == 'S')
		{
			GenerateObstacles();
			RenderGrid();
		}
		
		if (chKey == 'Z')
			AStar();
		/*switch(chKey)
		{
		}*/
	}

	return 0;
}
// Grid with start,end, obstacles
void RenderGrid()
{
	system("cls");
	for(int i=0; i< MAX_Y;i++)
	{
		for(int k=0; k< MAX_X;k++)
		{
			Pole pole;
			pole.X = k;
			pole.Y = i;
			if(start.X == k && start.Y == i)
				printf("A");
			else if(IsGoal(pole))
				printf("B");
			else if(IsObstacle(pole))
				printf("X");
			else
				printf("*");
		} 
		printf("\n");
	}
}

// Grid with path
void RenderGrid(Pole cameFrom[MAX_Y][MAX_X], Pole current)
{
	system("cls");
	char nGrid[MAX_Y][MAX_X];

	for (int i = 0; i < MAX_Y; i++)
	{
		for (int k = 0; k < MAX_X; k++)
		{
			Pole pole;
			pole.X = k;
			pole.Y = i;
			if (IsObstacle(pole))
				nGrid[i][k] = 'X';
			else
				nGrid[i][k] = '*';
		}
	}

	if(!IsGoal(current))
		nGrid[finish.Y][finish.X] = 'B';

	while(current.X != start.X || current.Y != start.Y)
	{
		if(IsGoal(current))
			nGrid[current.Y][current.X] = 'B';
		else
			nGrid[current.Y][current.X] = 254; // white box character
		current = cameFrom[current.Y][current.X];
	}
	nGrid[current.Y][current.X] = 'A';
	for(int i=0; i<MAX_Y;i++)
	{
		for(int k=0; k<MAX_X;k++)
			printf("%c", nGrid[i][k]);
		printf("\n");
	}
	Sleep(100);
}