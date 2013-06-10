#pragma once

class Cup {
public:
	int id;
	float x;
	float y;
	float z;

	Cup() {}

	Cup(float xPosition, float yPosition, float zPosition) {
		x = xPosition;
		y = yPosition;
		z = zPosition;
	}

	void setPos(float xPosition, float yPosition, float zPosition) {
		x = xPosition;
		y = yPosition;
		z = zPosition;
	}

private:
};