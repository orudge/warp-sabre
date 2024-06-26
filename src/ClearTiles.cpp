
#include "SlippyMapTile.h"
#include "gbos1936/Gbos1936.h"
#include "StringUtils.h"
#include "Tile.h"
#include "ImgMagick.h"
#include "ImageWarpByFunc.h"
#include "ganzc/LatLong-OSGBconversion.h"
#include "ReadKmlFile.h"
#include "OSTN02Perl.h"
#include "ReadDelimitedFile.h"
#include "GetBounds.h"
#include "SourceKml.h"
#include "ProgramOptions.h"

// #include <wand/magick-wand.h>
// #include <wand/drawing-wand.h>
// #include <Magick++.h>
// rsync --size-only -v -r /var/www/os7 timsc@dev.openstreetmap.org:/home/ooc
// rsync --size-only -v -r /var/www/openlayers/os7 timsc@dev.openstreetmap.org:/home/timsc/public_html/openlayers

#include <list>
#include <iostream>
#include <exception>
#include <vector>
// using namespace Magick;

//************************************

void DrawMarkerPix(class ImgMagick &img, int x, int y, double r, double g, double b)
{
	if (x < 0 || x >= img.GetWidth())
		return;
	if (y < 0 || y >= img.GetHeight())
		return;
	img.SetPix(x, y, 0, r);
	img.SetPix(x, y, 1, g);
	img.SetPix(x, y, 2, b);
}

void DrawMarker(class ImgMagick &img, double x, double y)
{
	// Draw to tile
	for (int i = -1; i <= 1; i++)
		for (int j = -1; j <= 1; j++)
		{
			if (i != 0 || j != 0)
				DrawMarkerPix(img, x + (double)i + 0.5, y + (double)j + 0.5, 0.0, 0.0, 0.0);
			else
				DrawMarkerPix(img, x + (double)i + 0.5, y + (double)j + 0.5, 255.0, 0.0, 255.0);
		}
}

//***************************************

int main(int argc, char **argv)
{
	ImgMagick::Init();

	// Image imageInOut("step.jpg");
	// imageInOut.crop( Geometry(255,255,0,0) );
	// imageInOut.write("test2.png");

	/*class Tile test;
	vector<string> test2;
	string test3;
	int retOpen = ReadKmlFile("/home/tim/dev/os7files/rect/73.kml", test, test3, test2);

	exit(0);*/

	class Tile dst;
	// cout << long2tile(-3.68, zoom) << "," << lat2tile(54.8333,zoom) << endl;
	// cout << long2tile(-3.04, zoom) << "," << lat2tile(55.2446,zoom) << endl;

	// Process program options
	std::stringstream desc;
	desc << "Allowed options" << endl;
	desc << "  --bounds arg          Bounds filename" << endl;
	desc << "  --output arg          Output folder" << endl;
	desc << "  --positional arg   	 Input KML files" << endl;
	desc << "  --help                help message" << endl;

	ProgramOptions po(argc, argv);

	string boundsFilename = "bounds.csv";
	string outFolder = "out";
	if (po.HasArg("help"))
	{
		cout << desc.str() << endl;
		exit(0);
	}
	if (po.HasArg("output"))
		outFolder = po.GetArg("output");
	if (po.HasArg("bounds"))
		boundsFilename = po.GetArg("bounds");

	vector<string> inputFiles;
	if (po.HasArg(NULL))
	{
		inputFiles = po.GetMultiArg(NULL);
		for (unsigned int i = 0; i < inputFiles.size(); i++)
			cout << "input file " << inputFiles[i] << endl;
	}

	vector<class SourceKml> src;
	class DelimitedFile boundsFile;
	int boundsOpen = boundsFile.Open(boundsFilename.c_str());
	if (boundsOpen < 1)
		cout << "Could not read bounds file " << boundsFilename << endl;

	class Tile sourceBBox;
	int sourceBBoxSet = 0;

	// For each input file, parse KML into local mem
	for (int i = 0; i < static_cast<int>(inputFiles.size()); i++)
	{
		class SourceKml temp;
		cout << "Source file '" << inputFiles[i] << "'" << endl;
		string filePath = GetFilePath(inputFiles[i].c_str());

		src.push_back(temp);
		class SourceKml &last = src[src.size() - 1];
		string imgFilename;
		int ret = ReadKmlFile(inputFiles[i].c_str(), last.tile, imgFilename);
		last.imgFilename = filePath;
		last.imgFilename += "/";
		last.imgFilename += imgFilename;
		if (ret < 1)
		{
			cout << "Kml " << inputFiles[i] << " not found";
			exit(0);
		}
		cout << last.tile.latmin << "," << last.tile.lonmin << "," << last.tile.latmax << "," << last.tile.lonmax << endl;
		cout << "image filename '" << last.imgFilename << "'" << endl;

		// Update source bounding box
		if (sourceBBox.latmin > last.tile.latmin || !sourceBBoxSet)
			sourceBBox.latmin = last.tile.latmin;
		if (sourceBBox.latmax < last.tile.latmax || !sourceBBoxSet)
			sourceBBox.latmax = last.tile.latmax;
		if (sourceBBox.lonmin > last.tile.lonmin || !sourceBBoxSet)
			sourceBBox.lonmin = last.tile.lonmin;
		if (sourceBBox.lonmax < last.tile.lonmax || !sourceBBoxSet)
			sourceBBox.lonmax = last.tile.lonmax;
		sourceBBoxSet = 1;

		// int ret = last.image.Open(last.imgFilename.c_str());
		// if(ret < 0){cout << "Filed to open image" << endl;exit(0);}
		// last.bounds.push_back("NR550000");
		// last.bounds.push_back("NR950450");
		string filenameNoPath = RemoveFilePath(argv[i]);
		vector<string> boundsTemp;
		string projType;
		GetBounds(boundsFile, filenameNoPath.c_str(), boundsTemp, projType);
		cout << "bounds (" << boundsTemp.size() << ")";
		for (unsigned int j = 0; j < boundsTemp.size(); j++)
			cout << projType << " : " << boundsTemp[j] << ",";
		cout << endl;
		last.bounds = boundsTemp;
	}

	cout << "Input files bounding box:" << endl;
	cout << sourceBBox.latmin << "," << sourceBBox.lonmin << "," << sourceBBox.latmax << "," << sourceBBox.lonmax << endl;

	class ImgMagick tile;
	tile.SetNumChannels(3);
	tile.SetWidth(256);
	tile.SetHeight(256);
	class ImgMagick outImg;
	outImg.SetNumChannels(3);
	outImg.SetWidth(256);
	outImg.SetHeight(256);

	// int zoom = 14;
	for (unsigned int zoom = 17; zoom >= 2; zoom--)
	{
		int srcWtile = long2tile(/*-3.68423*/ sourceBBox.lonmin, zoom);
		int srcEtile = long2tile(sourceBBox.lonmax, zoom);
		int srcStile = lat2tile(sourceBBox.latmin, zoom);
		int srcNtile = lat2tile(sourceBBox.latmax, zoom);
		// cout << "tiles covered " << srcStile << "," << srcWtile << "," << srcNtile << "," << srcEtile << endl;

		// int tileLon = 8024;
		// int tileLat = 5168;
		// For each tile to generate
		for (int tileLon = srcWtile; tileLon <= srcEtile; tileLon++)
			for (int tileLat = srcNtile; tileLat <= srcStile; tileLat++)
			{

				dst.latmax = tile2lat(tileLat, zoom);
				dst.lonmin = tile2long(tileLon, zoom);
				dst.latmin = tile2lat(tileLat + 1, zoom);
				dst.lonmax = tile2long(tileLon + 1, zoom);
				dst.sx = outImg.GetWidth();
				dst.sy = outImg.GetHeight();

				string outFilename = outFolder;
				string outFolder0 = outFilename;
				outFilename += "/";
				outFilename += IntToString(zoom);
				string outFolder1 = outFilename;
				outFilename += "/";
				outFilename += IntToString(tileLon);
				string outFolder2 = outFilename;
				outFilename += "/";
				outFilename += IntToString(tileLat);
				outFilename += ".jpg";

				if (fileExists(outFilename.c_str()))
				{
					cout << "Deleting: " << outFilename << endl;
					remove(outFilename.c_str());
				}

			} // End of tile loop
	} // End of zoom loop

	ImgMagick::Term();
}
