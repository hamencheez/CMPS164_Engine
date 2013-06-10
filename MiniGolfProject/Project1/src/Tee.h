#pragma once

class Tee {
public:
	int id;
	float x;
	float y;
	float z;

	Tee() {}

	Tee(float xPosition, float yPosition, float zPosition) {
		x = xPosition;
		y = yPosition;
		z = zPosition;
	}

private:
};