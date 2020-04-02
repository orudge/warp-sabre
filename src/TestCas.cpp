#include "gbos1936/Gbos1936.h"
#include <iostream>
#include <math.h>

HelmertConverter gConverter;

int main()
{
  double e = 0.0, n = 0.0, h = 0.0;
  gConverter.ConvertWgs84ToCas(53.22175203,-2.68569717,0.0,e,n);
  std::cout << ( e * 5280 ) << "," << ( n * 5280 ) << std::endl;
  gConverter.ConvertWgs84ToCas(53.04493168,-2.74724954,0.0,e,n);
  std::cout << ( e * 5280 ) << "," << ( n * 5280 ) << std::endl;

  double lat = 0.0, lon = 0.0, alt = 0.0;
  gConverter.ConvertCasToWgs84(0.0,0.0,0.0,lat, lon, alt );
  std::cout << lat << "," << lon << std::endl;
  gConverter.ConvertCasToWgs84(100.0,100.0,0.0,lat, lon, alt );
  std::cout << lat << "," << lon << std::endl;

  gConverter.ConvertWgs84ToBnS(57.49973044,-4.00138586,0.0,e,n);
  std::cout << ( e * 5280 ) << "," << ( n * 5280 ) << std::endl;


  gConverter.ConvertBnSToWgs84(0.0,0.0,0.0, lat, lon, alt );
  std::cout << lat << "," << lon << std::endl;
  gConverter.ConvertBnSToWgs84(100.0,100.0,0.0, lat, lon, alt );
  std::cout << lat << "," << lon << std::endl;

  gConverter.ConvertOsiToWgs84(200000,250000,0.0, lat, lon, alt );
  std::cout << lat << "," << lon << std::endl;
  gConverter.ConvertWgs84ToOsi(53.5,-8,0.0, e, n, h);
  std::cout << e << "," << n << std::endl;

  return 0;
}
