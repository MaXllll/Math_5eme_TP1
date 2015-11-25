#pragma once

struct Model
{
	enum SplineMode
	{
		CREATEPOINT,
		MOVEPOINT,
	};

	enum Mode
	{
		GRAHAMSCAN,
		TRIANGULATION,
	};

	bool wireFrame = true;
	bool extrusionType = false;

	float degree;
	float pas;
	SplineMode splineMode;
	Mode mode;
};