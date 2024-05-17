#include "gbos1936/Gbos1936.h"
#include "StringUtils.h"
#include "ReadKmlFile.h"
#include "SourceKml.h"
#include "ReadDelimitedFile.h"
#include "GetBounds.h"
#include "CopyPixels.h"
#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <memory>

#include <vector>
using namespace std;

void ClearImage( const string& srcFilename, DelimitedFile& boundsFile, const int edge )
{
	SourceKml srcKml;
	cout << "Source file '" << srcFilename << "'" << endl;
	string imgFilename;
	int ret = ReadKmlFile( srcFilename.c_str(), srcKml.tile, imgFilename );
	srcKml.imgFilename = GetFilePath( srcFilename.c_str() );
	srcKml.imgFilename += "/";
	srcKml.imgFilename += imgFilename;
	if( ret < 1 ) {
		cout << "Kml " << srcFilename << " not found";
		return;
	}
	cout << srcKml.tile.latmin << "," << srcKml.tile.lonmin << "," << srcKml.tile.latmax << "," << srcKml.tile.lonmax << endl;	
	cout << "Image filename '" << srcKml.imgFilename << "'" << endl;

	vector<string> bounds;
	string projType;
	GetBounds( boundsFile, RemoveFilePath( srcFilename.c_str() ).c_str(), bounds, projType );
	cout << projType << " bounds (" << bounds.size() << ")" << endl;
	srcKml.bounds = bounds;
	srcKml.projType = projType;
	srcKml.tile.sx = srcKml.image.GetWidth();
	srcKml.tile.sy = srcKml.image.GetHeight();

	ret = srcKml.image.Open( srcKml.imgFilename.c_str() );
	if ( ret < 0 ) {
		cout << "Failed to open image " << srcKml.imgFilename << endl;
		return;
	}

	srcKml.tile.sx = srcKml.image.GetWidth();
	srcKml.tile.sy = srcKml.image.GetHeight();

	cout << "Output filename '" << imgFilename.c_str() << "' size " << srcKml.tile.sx << "x" << srcKml.tile.sy << endl;

	unique_ptr<CopyPixels> mask( CopyPixels::Create( srcKml.projType.c_str() ) );
	for( unsigned int i = 0; i < srcKml.bounds.size(); i++ ) {
		mask->UpdateBoundingBox( srcKml.bounds[ i ].c_str() );
	}
	mask->FastCopy( srcKml.image, srcKml.image, srcKml.tile, edge );

	cout << "Saving file...." << endl;

	srcKml.image.Save( srcKml.imgFilename.c_str() );
}

int main( int argc, char* argv[])
{
	ImgMagick::Init();

	vector<SourceKml> src;
	DelimitedFile boundsFile;

	string boundsFilename = "bounds.csv";
        int edge = 128;
	vector<string> inputFiles;
	string execFilename = "clearbounds";
	if( argc >= 1 ) execFilename = argv[ 0 ];

	po::variables_map vm;
	po::options_description desc( "Allowed options" );
	try {
		po::positional_options_description pd;
		pd.add( "positional", -1 );
		desc.add_options() ( "bounds", po::value<string>(), "Bounds filename" )
		( "edge", po::value<int>(), "Number of pixels from the edge to check (higher = more accurate but slower)" )
		( "positional" , po::value<vector<string> >(), "Input KML file" )
		( "help", "help message" );
		po::parsed_options parsed = po::command_line_parser(argc, argv ).options( desc ).allow_unregistered().positional( pd ).run();
		po::store( parsed, vm );
		po::notify( vm );

		if(vm.count("help")) { 
			cout << desc << endl;
			exit( 0 );
		}
		if ( vm.count( "bounds" ) ) {
			boundsFilename = vm["bounds"].as<string>();
		}
		if ( vm.count( "edge" ) ) {
			edge = vm["edge"].as<int>();
		}
		if ( vm.count("positional") ) {
			inputFiles = vm["positional"].as<vector<string> >();
		}
	} catch( exception& e ) {
		cerr << "error: " << e.what() << endl;
	}

	if( 0 == inputFiles.size() ) {
		cout << "Input files not specified" << endl;
		cout << "Usage: " << execFilename << " [options] kml_files" << endl;
		cout << desc << endl;
		return 0;
	}

	int boundsOpen = boundsFile.Open( boundsFilename.c_str() );
	if( boundsOpen < 1 ) {
		cout << "Could not read " << boundsFilename << " file" << endl;
	} else {
		for( vector<string>::iterator it = inputFiles.begin();
			it != inputFiles.end(); it++ ){
			ClearImage( *it, boundsFile, edge );
		}
	}
	ImgMagick::Term();
	return 1;
}
