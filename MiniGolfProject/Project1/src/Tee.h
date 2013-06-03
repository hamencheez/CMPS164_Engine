#pragma once

class Tee {
public:
	int id;
	float x;
	float y;
	float z;

	Tee() {

	}

	Tee(int tileId, float xPosition, float yPosition, float zPosition) {
		id = tileId;
		x = xPosition;
		y = yPosition;
		z = zPosition;
	}
private:
};