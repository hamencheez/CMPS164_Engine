#pragma once

class Cup {
public:
	int id;
	float x;
	float y;
	float z;

	Cup() {	}

	Cup(int tileId, float xPosition, float yPosition, float zPosition) {
		id = tileId;
		x = xPosition;
		y = yPosition;
		z = zPosition;
	}

private:
};