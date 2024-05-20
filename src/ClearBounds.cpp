#include "gbos1936/Gbos1936.h"
#include "StringUtils.h"
#include "ReadKmlFile.h"
#include "SourceKml.h"
#include "ReadDelimitedFile.h"
#include "GetBounds.h"
#include "CopyPixels.h"
#include "ProgramOptions.h"

#include <memory>

#include <vector>
using namespace std;

void ClearImage(const string &srcFilename, DelimitedFile &boundsFile, const int edge)
{
	SourceKml srcKml;
	cout << "Source file '" << srcFilename << "'" << endl;
	string imgFilename;
	int ret = ReadKmlFile(srcFilename.c_str(), srcKml.tile, imgFilename);
	srcKml.imgFilename = GetFilePath(srcFilename.c_str());
	srcKml.imgFilename += "/";
	srcKml.imgFilename += imgFilename;
	if (ret < 1)
	{
		cout << "Kml " << srcFilename << " not found";
		return;
	}
	cout << srcKml.tile.latmin << "," << srcKml.tile.lonmin << "," << srcKml.tile.latmax << "," << srcKml.tile.lonmax << endl;
	cout << "Image filename '" << srcKml.imgFilename << "'" << endl;

	vector<string> bounds;
	string projType;
	GetBounds(boundsFile, RemoveFilePath(srcFilename.c_str()).c_str(), bounds, projType);
	cout << projType << " bounds (" << bounds.size() << ")" << endl;
	srcKml.bounds = bounds;
	srcKml.projType = projType;
	srcKml.tile.sx = srcKml.image.GetWidth();
	srcKml.tile.sy = srcKml.image.GetHeight();

	ret = srcKml.image.Open(srcKml.imgFilename.c_str());
	if (ret < 0)
	{
		cout << "Failed to open image " << srcKml.imgFilename << endl;
		return;
	}

	srcKml.tile.sx = srcKml.image.GetWidth();
	srcKml.tile.sy = srcKml.image.GetHeight();

	cout << "Output filename '" << imgFilename.c_str() << "' size " << srcKml.tile.sx << "x" << srcKml.tile.sy << endl;

	unique_ptr<CopyPixels> mask(CopyPixels::Create(srcKml.projType.c_str()));
	for (unsigned int i = 0; i < srcKml.bounds.size(); i++)
	{
		mask->UpdateBoundingBox(srcKml.bounds[i].c_str());
	}
	mask->FastCopy(srcKml.image, srcKml.image, srcKml.tile, edge);

	cout << "Saving file...." << endl;

	srcKml.image.Save(srcKml.imgFilename.c_str());
}

int main(int argc, char *argv[])
{
	ImgMagick::Init();

	vector<SourceKml> src;
	DelimitedFile boundsFile;

	string boundsFilename = "bounds.csv";
	int edge = 128;
	vector<string> inputFiles;
	string execFilename = "clearbounds";
	if (argc >= 1)
		execFilename = argv[0];

	std::stringstream desc;
	desc << "Allowed options" << endl;
	desc << "  --bounds arg          Bounds filename" << endl;
	desc << "  --edge arg      		 Number of pixels from the edge to check (higher = more accurate but slower)" << endl;
	desc << "  --positional arg  	 Input KML files" << endl;
	desc << "  --help                help message" << endl;

	ProgramOptions po(argc, argv);

	if (po.HasArg("help"))
	{
		cout << desc.str() << endl;
		exit(0);
	}
	if (po.HasArg("bounds"))
	{
		boundsFilename = po.GetArg("bounds");
	}
	if (po.HasArg("edge"))
	{
		edge = po.GetIntArg("edge");
	}
	if (po.HasArg(NULL))
	{
		inputFiles = po.GetMultiArg(NULL);
	}

	if (0 == inputFiles.size())
	{
		cout << "Input files not specified" << endl;
		cout << "Usage: " << execFilename << " [options] kml_files" << endl;
		cout << desc.str() << endl;
		return 0;
	}

	int boundsOpen = boundsFile.Open(boundsFilename.c_str());
	if (boundsOpen < 1)
	{
		cout << "Could not read " << boundsFilename << " file" << endl;
	}
	else
	{
		for (vector<string>::iterator it = inputFiles.begin();
			 it != inputFiles.end(); it++)
		{
			ClearImage(*it, boundsFile, edge);
		}
	}
	ImgMagick::Term();
	return 1;
}
