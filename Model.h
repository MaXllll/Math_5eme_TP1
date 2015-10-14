#pragma once

struct Model
{
	enum SplineMode
	{
		CREATEPOINT,
		MOVEPOINT,
		REPEATPOINT
	};

	enum Mode
	{
		BSPLINE,
		EXTRUSION,
		BSURFACE
	};

	bool wireFrame = false;
	bool extrusionType = false;

	float degree;
	float pas;
	SplineMode splineMode;
	Mode mode;
};