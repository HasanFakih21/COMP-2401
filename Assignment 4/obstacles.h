typedef struct {
	int x;
	int y;
	int w;
	int h;
} Obstacle;

typedef struct {
	int x;
	int y;
	struct Neighbour *firstNeighbour;
	struct Neighbour *lastNeighbour;
	Obstacle *obstacle;
} Vertex;

typedef struct {
	Vertex *vertex;
	struct Neighbour *next;
} Neighbour;

typedef struct {
	int numObstacles;
	int numVertices;
	Vertex *vertices;
	Obstacle *obstacles;
} Environment;
