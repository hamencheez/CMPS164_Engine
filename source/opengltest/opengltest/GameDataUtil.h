

typedef struct __position__ {

	float x, y, z;
}POS;

typedef struct __vertex__ {
	POS* start;
	POS* end;

}VERT;

typedef struct __tile__{
	int ID, points, neigh[20];
	float V[20][3];
	TILE* next;
}TILE;

typedef struct __tee__{
	int ID;
	float position[3];
}TEE;

typedef struct __ball__{
	float position[3], dir[3], speed;
	int tile_ID;
}BALL;


typedef struct __map__{
 TILE* tiles;
 TEE* tee;
 BALL* ball;
 TEE* cup;
 int par, hits;
 char* mapName;
 MAP* next_level;
}MAP;