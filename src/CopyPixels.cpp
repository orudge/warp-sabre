#include "gbos1936/Gbos1936.h"
#include "ImgMagick.h"
#include <vector>
#include <iostream>
#include <stdio.h>
#include "CopyPixels.h"
#include "Tile.h"
#include "ganzc/LatLong-OSGBconversion.h"

using namespace std;

class HelmertConverter gConverter;

CopyPixels *CopyPixels::Create(const char *type)
{
	CopyPixels *mask = NULL;
	switch (type[0])
	{
	case 'G':
		mask = new CopyPixelsWithOsMask();
		break;
	case 'O':
		mask = new CopyPixelsWithRawMask();
		break;
	case 'C':
		mask = new CopyPixelsWithCassini();
		break;
	case 'B':
		mask = new CopyPixelsWithBonne();
		break;
	case 'I':
		mask = new CopyPixelsWithOsI();
		break;
	case 'R':
		mask = new CopyPixelsWithIrishBonne();
		break;
	case 'M':
		mask = new CopyPixelsWithMercator();
		break;
	default:
		cout << "Invalid projection type " << type << endl;
		throw 3001;
		break;
	}
	return mask;
}

CopyPixels::CopyPixels()
{
	gnorth = 0;
	gsouth = 0;
	geast = 0;
	gwest = 0;
	boxset = 0;
}

CopyPixels::~CopyPixels()
{
}

void CopyPixels::ThrowError(const char *what, const char *ref)
{
	cout << "Error : " << what << " " << ref << endl;
	throw 3000;
}

void CopyPixelsWithOsMask::UpdateBoundingBox(const char *mapref)
{
	int dEasting = 0, dNorthing = 0;
	if (2 == sscanf(mapref, "%d:%d", &dEasting, &dNorthing))
	{
		if (!boxset || gsouth > dNorthing)
			gsouth = dNorthing;
		if (!boxset || gnorth < dNorthing)
			gnorth = dNorthing;
		if (!boxset || geast < dEasting)
			geast = dEasting;
		if (!boxset || gwest > dEasting)
			gwest = dEasting;
		boxset = 1;
		// double lat=-1.0, lon=-1.0, alt = -1.0;
		// ConvertGbos1936ToWgs84(dEasting, dNorthing,0.0, lat, lon, alt);
	}
}

void CopyPixelsWithRawMask::UpdateBoundingBox(const char *mapref)
{
	int dEasting = 0, dNorthing = 0;
	if (2 == sscanf(mapref, "%d:%d", &dEasting, &dNorthing))
	{
		if (!boxset || gsouth > dNorthing)
			gsouth = dNorthing;
		if (!boxset || gnorth < dNorthing)
			gnorth = dNorthing;
		if (!boxset || geast < dEasting)
			geast = dEasting;
		if (!boxset || gwest > dEasting)
			gwest = dEasting;
		boxset = 1;
	}
	else
	{
		ThrowError("Invalid raw OS map reference", mapref);
	}
}

void CopyPixelsWithCassini::UpdateBoundingBox(const char *mapref)
{
	double dEasting = 0, dNorthing = 0;
	if (2 == sscanf(mapref, "%lf:%lf", &dEasting, &dNorthing))
	{
		if (!boxset || gsouth > dNorthing)
			gsouth = dNorthing;
		if (!boxset || gnorth < dNorthing)
			gnorth = dNorthing;
		if (!boxset || geast < dEasting)
			geast = dEasting;
		if (!boxset || gwest > dEasting)
			gwest = dEasting;
		boxset = 1;
	}
	else
	{
		ThrowError("Invalid Cassini map reference", mapref);
	}
}

void CopyPixelsWithBonne::UpdateBoundingBox(const char *mapref)
{
	double dEasting = 0, dNorthing = 0;
	if (2 == sscanf(mapref, "%lf:%lf", &dEasting, &dNorthing))
	{
		if (!boxset || gsouth > dNorthing)
			gsouth = dNorthing;
		if (!boxset || gnorth < dNorthing)
			gnorth = dNorthing;
		if (!boxset || geast < dEasting)
			geast = dEasting;
		if (!boxset || gwest > dEasting)
			gwest = dEasting;
		boxset = 1;
	}
	else
	{
		ThrowError("Invalid Bonne map reference", mapref);
	}
}

void CopyPixelsWithMercator::UpdateBoundingBox(const char *mapref)
{
	double dLng = 0, dLat = 0;
	if (2 == sscanf(mapref, "%lf:%lf", &dLat, &dLng))
	{
		if (!boxset || gsouth > dLat)
			gsouth = dLat;
		if (!boxset || gnorth < dLat)
			gnorth = dLat;
		if (!boxset || geast < dLng)
			geast = dLng;
		if (!boxset || gwest > dLng)
			gwest = dLng;
		gVertx.clear();
		gVerty.clear();

		gVertx.push_back(gwest);
		gVertx.push_back(geast);
		gVertx.push_back(geast);
		gVertx.push_back(gwest);

		gVerty.push_back(gnorth);
		gVerty.push_back(gnorth);
		gVerty.push_back(gsouth);
		gVerty.push_back(gsouth);

		boxset = 1;
	}
	else
	{
		ThrowError("Invalid mercator reference", mapref);
	}
}

int CopyPixelsWithOsMask::CheckIfInBox(double lat, double lon)
{
	double pnorth, peast, palt;
	gConverter.ConvertWgs84ToGbos1936(lat, lon, 0.0, peast, pnorth, palt);
	if (pnorth < gsouth)
		return 0;
	if (pnorth > gnorth)
		return 0;
	if (peast < gwest)
		return 0;
	if (peast > geast)
		return 0;

	return 1;
}

int CopyPixelsWithOsI::CheckIfInBox(double lat, double lon)
{
	double pnorth, peast, palt;
	gConverter.ConvertWgs84ToOsi(lat, lon, 0.0, peast, pnorth, palt);
	if (pnorth < gsouth)
		return 0;
	if (pnorth > gnorth)
		return 0;
	if (peast < gwest)
		return 0;
	if (peast > geast)
		return 0;

	return 1;
}

int CopyPixelsWithCassini::CheckIfInBox(double lat, double lon)
{
	double pnorth, peast;
	gConverter.ConvertWgs84ToCas(lat, lon, 0.0, peast, pnorth);
	if (pnorth < gsouth)
		return 0;
	if (pnorth > gnorth)
		return 0;
	if (peast < gwest)
		return 0;
	if (peast > geast)
		return 0;
	return 1;
}

int CopyPixelsWithBonne::CheckIfInBox(double lat, double lon)
{
	double pnorth, peast;
	gConverter.ConvertWgs84ToBnS(lat, lon, 0.0, peast, pnorth);
	if (pnorth < gsouth)
		return 0;
	if (pnorth > gnorth)
		return 0;
	if (peast < gwest)
		return 0;
	if (peast > geast)
		return 0;
	return 1;
}

int CopyPixelsWithIrishBonne::CheckIfInBox(double lat, double lon)
{
	double pnorth, peast;
	gConverter.ConvertWgs84ToBnI(lat, lon, 0.0, peast, pnorth);
	if (pnorth < gsouth)
		return 0;
	if (pnorth > gnorth)
		return 0;
	if (peast < gwest)
		return 0;
	if (peast > geast)
		return 0;
	return 1;
}

int CopyPixelsWithMercator::CheckIfInBox(double lat, double lon)
{
	int c = 0, i = 0, j = 0;
	for (i = 0, j = 3; i < 4; j = i++)
	{
		if (((gVerty[i] > lat) != (gVerty[j] > lat)) &&
			(lon < (gVertx[j] - gVertx[i]) * (lat - gVerty[i]) / (gVerty[j] - gVerty[i]) + gVertx[i]))
		{
			c = !c;
		}
	}
	return c;
}

void CopyPixels::FastCopy(class ImgMagick &imageIn, class ImgMagick &imageOut, class Tile &tile, const int tileSize)
{
	int width = imageIn.GetWidth();
	int height = imageIn.GetHeight();
	int chans = imageIn.GetNumChannels();

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			for (int k = 0; k < chans; k++)
			{
				if (i < tileSize ||
					j < tileSize ||
					i > (width - tileSize) ||
					j > (height - tileSize))
				{
					CopyTile(imageIn, imageOut, tile, i, j, k, false /* no merge */);
				}
			}
		}
	}
}

bool CopyPixels::CopyTile(class ImgMagick &imageIn, class ImgMagick &imageOut, class Tile &tile, const int i, const int j, const int k, const bool merge)
{
	double lat, lon;
	tile.UnProject(i, j, lat, lon);
	double val = imageIn.GetPix(i, j, k);
	int inMask = this->CheckIfInBox(lat, lon);
	if (inMask)
	{
		if (!merge || (val > 0))
		{
			imageOut.SetPix(i, j, k, val);
			return true;
		}
	}
	return false;
}

void CopyPixels::Copy(class ImgMagick &imageIn, class ImgMagick &imageOut, class Tile &tile, const bool merge)
{
	int width = imageIn.GetWidth();
	int height = imageIn.GetHeight();
	int chans = imageIn.GetNumChannels();

	// Check corners (quicker to check then exhaustively check each pixel)
	double lat, lon;
	tile.UnProject(0, 0, lat, lon);
	int cornerCheck = this->CheckIfInBox(lat, lon);
	tile.UnProject(width, 0, lat, lon);
	cornerCheck = cornerCheck && this->CheckIfInBox(lat, lon);
	tile.UnProject(width, height, lat, lon);
	cornerCheck = cornerCheck && this->CheckIfInBox(lat, lon);
	tile.UnProject(0, height, lat, lon);
	cornerCheck = cornerCheck && this->CheckIfInBox(lat, lon);
	// cout << cornerCheck << endl;
	if (cornerCheck || boxset == 0) // Entire tile is in mask
	{

		// Copy every pixel
		for (int i = 0; i < width; i++)
			for (int j = 0; j < height; j++)
				for (int k = 0; k < chans; k++)
				{
					double val = imageIn.GetPix(i, j, k);
					if (!merge || val)
					{
						imageOut.SetPix(i, j, k, val);
					}
				}
	}
	else
	{
		// Check each pixel and copy if inside mask
		for (int i = 0; i < width; i++)
			for (int j = 0; j < height; j++)
				for (int k = 0; k < chans; k++)
				{
					CopyTile(imageIn, imageOut, tile, i, j, k, merge);
				}
	}
}
