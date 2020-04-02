#include <string>
#include <iostream>
#include "Tile.h"
#include "ReadKmlFile.h"
#include "ImgMagick.h"
#include "StringUtils.h"
using namespace std;

int main( int argc, char* argv[] )
{
  for( int i = 1; i < argc; i++ ) {
    Tile t;
    string file;
    const char* kmlFile = argv[ i ];
    if( ReadKmlFile( kmlFile, t, file ) ) {
      ImgMagick img;
      string filePath = GetFilePath( kmlFile );
      filePath += "/";
      filePath += file;
      if( img.Open( filePath.c_str() ) ) {
        cout << ( t.lonmax - t.lonmin ) / img.GetWidth() << endl;
        cout << "0.0" << endl;
        cout << "0.0" << endl;
        cout << ( t.latmax - t.latmin ) / img.GetHeight() << endl;
        cout << t.lonmin << endl;
        cout << t.latmin << endl;
        img.Close();
      }
    }
  }
}
