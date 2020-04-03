#include "ImgMagick.h"
#include <time.h>

class SourceKml
{
public:
	class Tile tile;
	class ImgMagick image;
	vector<string> bounds;
        string kmlFilename;
	string imgFilename;
	clock_t lastAccess;
        int maxZoomVisible;
	string projType;

	SourceKml() {lastAccess = 0; maxZoomVisible=-1; projType="M"; }
	SourceKml(const SourceKml &a) {operator=(a);}
	virtual ~SourceKml() {}
	SourceKml& operator=(const SourceKml& a)
	{
		tile = a.tile;
		image = a.image;
		bounds = a.bounds;
                kmlFilename = a.kmlFilename;
		imgFilename = a.imgFilename;
		lastAccess = a.lastAccess;
		maxZoomVisible = a.maxZoomVisible;
		projType = a.projType;
		return *this;
	}
};

