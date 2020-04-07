
//Helmert approximation of GBOS1936 to WGS84
//Accurate to 6 metres approx
//Based on that VB Excel OS ProjectionandTransformationCalculations.xls
//Probably based on OS Crown Copyright
//Ported to C++ by Tim Sheerman-Chase 2009
//Cassini Delamere and Bonne projections added by Ritchie Swann 2010
//using algorithms derived from www.fieldenmaps.info

#include <math.h>
#include <iostream>
using namespace std;
#include "Gbos1936.h"

double Marc(double bf0, double n, double PHI0, double PHI);
double InitialLat(double North, double n0, double afo, double PHI0, double n, double bfo);
double Iterate_XYZ_to_Lat(double a,double e2,double PHI1,double Z,double RootXYSqr);

/*'********************************************************************************************* 
 THE FUNCTIONS IN THIS MODULE ARE WRITTEN TO BE "STAND ALONE" WITH AS LITTLE DEPENDANCE 
 ON OTHER FUNCTIONS AS POSSIBLE.  THIS MAKES THEM EASIER TO COPY TO OTHER VB APPLICATIONS. 
 WHERE A FUNCTION DOES CALL ANOTHER FUNCTION THIS IS STATED IN THE COMMENTS AT THE START OF 
 THE FUNCTION. 
 **********************************************************************************************/

double Helmert_X(double X, double Y, double Z, double DX, double Y_Rot, double Z_Rot, double s)
{
/*'Computed Helmert transformed X coordinate.
'Input: - 
 cartesian XYZ coords (X,Y,Z), X translation (DX) all in meters ; 
 Y and Z rotations in seconds of arc (Y_Rot, Z_Rot) and scale in ppm (s).*/
 
//'Convert rotations to radians and ppm scale to a factor
    double Pi = 3.14159265358979;
    double sfactor = s * 0.000001;
    double RadY_Rot = (Y_Rot / 3600.0) * (Pi / 180.0);
    double RadZ_Rot = (Z_Rot / 3600.0) * (Pi / 180.0);
    
//Compute transformed X coord
    return X + (X * sfactor) - (Y * RadZ_Rot) + (Z * RadY_Rot) + DX;

}


double Helmert_Y(double X, double Y, double Z, double DY, double X_Rot, double Z_Rot, double s)
{
/*'Computed Helmert transformed Y coordinate.
'Input: - 
 cartesian XYZ coords (X,Y,Z), Y translation (DY) all in meters ; 
 X and Z rotations in seconds of arc (X_Rot, Z_Rot) and scale in ppm (s).*/
 
//Convert rotations to radians and ppm scale to a factor
    double Pi = 3.14159265358979;
    double sfactor = s * 0.000001;
    double RadX_Rot = (X_Rot / 3600.0) * (Pi / 180.0);
    double RadZ_Rot = (Z_Rot / 3600.0) * (Pi / 180.0);
    
//Compute transformed Y coord
    return (X * RadZ_Rot) + Y + (Y * sfactor) - (Z * RadX_Rot) + DY;
 
}

double Helmert_Z(double X, double Y, double Z, double DZ,double  X_Rot,double  Y_Rot,double  s)
{
/*'Computed Helmert transformed Z coordinate.
'Input: - 
 cartesian XYZ coords (X,Y,Z), Z translation (DZ) all in meters ; 
 X and Y rotations in seconds of arc (X_Rot, Y_Rot) and scale in ppm (s).*/
 
//Convert rotations to radians and ppm scale to a factor
    double Pi = 3.14159265358979;
    double sfactor = s * 0.000001;
    double RadX_Rot = (X_Rot / 3600.0) * (Pi / 180.0);
    double RadY_Rot = (Y_Rot / 3600.0) * (Pi / 180.0);
    
//Compute transformed Z coord
    return (-1.0 * X * RadY_Rot) + (Y * RadX_Rot) + Z + (Z * sfactor) + DZ;
 
}

double XYZ_to_Lat(double X, double Y, double Z, double a, double b)
{
/*'Convert XYZ to Latitude (PHI) in Dec Degrees.
'Input: - 
 XYZ cartesian coords (X,Y,Z) and ellipsoid axis dimensions (a & b), all in meters.

'THIS FUNCTION REQUIRES THE "Iterate_XYZ_to_Lat" FUNCTION
'THIS FUNCTION IS CALLED BY THE "XYZ_to_H" FUNCTION*/

    double RootXYSqr = sqrt((X * X) + (Y * Y));
    double e2 = ((a * a) - (b * b)) / (a * a);
    double PHI1 = atan(Z / (RootXYSqr * (1.0 - e2)));
    
    double PHI = Iterate_XYZ_to_Lat(a, e2, PHI1, Z, RootXYSqr);
    
    double Pi = 3.14159265358979;
    
    return PHI * (180.0 / Pi);

}

double Iterate_XYZ_to_Lat(double a,double e2,double PHI1,double Z,double RootXYSqr)
{
/*'Iteratively computes Latitude (PHI).
'Input: - 
 ellipsoid semi major axis (a) in meters; 
 eta squared (e2); 
 estimated value for latitude (PHI1) in radians; 
 cartesian Z coordinate (Z) in meters; 
 RootXYSqr computed from X & Y in meters.

'THIS FUNCTION IS CALLED BY THE "XYZ_to_PHI" FUNCTION
'THIS FUNCTION IS ALSO USED ON IT'S OWN IN THE 
 "Projection and Transformation Calculations.xls" SPREADSHEET*/

    double V = a / (sqrt(1.0 - (e2 * pow((sin(PHI1)) ,2.0))));
    double PHI2 = atan((Z + (e2 * V * (sin(PHI1)))) / RootXYSqr);
    
    while (fabs(PHI1 - PHI2) > 0.000000001)
    {
        PHI1 = PHI2;
        V = a / (sqrt(1.0 - (e2 * pow((sin(PHI1)) , 2.0))));
        PHI2 = atan((Z + (e2 * V * (sin(PHI1)))) / RootXYSqr);
    }

    return PHI2;

}

double XYZ_to_Long(double X,double Y)
{
/*'Convert XYZ to Longitude (LAM) in Dec Degrees.
'Input: - 
 X and Y cartesian coords in meters.*/

    double Pi = 3.14159265358979;
    return (atan(Y / X)) * (180.0 / Pi);

}

double XYZ_to_H(double X,double  Y,double  Z,double  a,double  b)
{
/*Convert XYZ to Ellipsoidal Height.
'Input: - 
 XYZ cartesian coords (X,Y,Z) and ellipsoid axis dimensions (a & b), all in meters.

'REQUIRES THE "XYZ_to_Lat" FUNCTION*/

//Compute PHI (Dec Degrees) first
    double PHI = XYZ_to_Lat(X, Y, Z, a, b);

//Convert PHI radians
    double Pi = 3.14159265358979;
    double RadPHI = PHI * (Pi / 180.0);
    
//Compute H
    double RootXYSqr = sqrt((X * X) + (Y * Y));
    double e2 = ((a * a) - (b * b)) / (a * a);
    double V = a / (sqrt(1.0 - (e2 * pow((sin(RadPHI)), 2.0))));
    double H = (RootXYSqr / cos(RadPHI)) - V;
    
    return H;
}


double Lat_Long_H_to_X(double PHI, double LAM, double H, double a, double b)
{
/*Convert geodetic coords lat (PHI), long (LAM) and height (H) to cartesian X coordinate.
'Input: - 
 Latitude (PHI)& Longitude (LAM) both in decimal degrees; 
 Ellipsoidal height (H) and ellipsoid axis dimensions (a & b) all in meters.*/

//Convert angle measures to radians
    double Pi = 3.14159265358979;
    double RadPHI = PHI * (Pi / 180);
    double RadLAM = LAM * (Pi / 180);

//Compute eccentricity squared and nu
    double e2 = ((a * a) - (b * b)) / (a * a);
    double V = a / (sqrt(1.0 - (e2 * pow((sin(RadPHI)), 2.0))));

//Compute X
    return (V + H) * (cos(RadPHI)) * (cos(RadLAM));

}

double Lat_Long_H_to_Y(double PHI, double LAM, double H, double a, double b)
{
/*Convert geodetic coords lat (PHI), long (LAM) and height (H) to cartesian Y coordinate.
'Input: - 
 Latitude (PHI)& Longitude (LAM) both in decimal degrees; 
 Ellipsoidal height (H) and ellipsoid axis dimensions (a & b) all in meters.*/

//Convert angle measures to radians
    double Pi = 3.14159265358979;
    double RadPHI = PHI * (Pi / 180);
    double RadLAM = LAM * (Pi / 180);

//Compute eccentricity squared and nu
    double e2 = ((a * a) - (b * b)) / (a * a);
    double V = a / (sqrt(1.0 - (e2 * pow((sin(RadPHI)), 2.0))));

//Compute Y
    return (V + H) * (cos(RadPHI)) * (sin(RadLAM));

}

double Lat_H_to_Z(double PHI, double LAM, double H, double a, double b)
{
/*Convert geodetic coord components latitude (PHI) and height (H) to cartesian Z coordinate.
'Input: - 
 Latitude (PHI) decimal degrees; 
 Ellipsoidal height (H) and ellipsoid axis dimensions (a & b) all in meters.*/

//Convert angle measures to radians
    double Pi = 3.14159265358979;
    double RadPHI = PHI * (Pi / 180);
    //double RadLAM = LAM * (Pi / 180);

//Compute eccentricity squared and nu
    double e2 = ((a * a) - (b * b)) / (a * a);
    double V = a / (sqrt(1.0 - (e2 * pow((sin(RadPHI)), 2.0))));

//Compute Z
   return ((V * (1 - e2)) + H) * (sin(RadPHI));

}

double Lat_Long_to_East(double PHI, double LAM, double a, double b, double e0, double f0, double PHI0, double LAM0)
{
/*
Function Lat_Long_to_East(PHI, LAM, a, b, e0, f0, PHI0, LAM0)
'Project Latitude and longitude to Transverse Mercator eastings.
'Input: - _
 Latitude (PHI) and Longitude (LAM) in decimal degrees; _
 ellipsoid axis dimensions (a & b) in meters; _
 eastings of false origin (e0) in meters; _
 central meridian scale factor (f0); _
 latitude (PHI0) and longitude (LAM0) of false origin in decimal degrees.
*/    //Convert angle measures to radians
    double Pi = 3.14159265358979;
    double RadPHI = PHI * (Pi / 180.0);
    double RadLAM = LAM * (Pi / 180.0);
    //double RadPHI0 = PHI0 * (Pi / 180.0);
    double RadLAM0 = LAM0 * (Pi / 180.0);

    double af0 = a * f0;
    double bf0 = b * f0;
    double e2 = ((af0 * af0) - (bf0 * bf0)) / (af0 * af0);
    //double n = (af0 - bf0) / (af0 + bf0);
    double nu = af0 / pow((1.0 - (e2 * pow((sin(RadPHI)), 2.0))),0.5);
    double rho = (nu * (1.0 - e2)) / (1.0 - pow(e2 * (sin(RadPHI)), 2.0));
    double eta2 = (nu / rho) - 1.0;
    double p = RadLAM - RadLAM0;
    
    double IV = nu * (cos(RadPHI));
    double V = (nu / 6.0) * pow((cos(RadPHI)), 3.0) * ((nu / rho) - (pow(tan(RadPHI), 2.0)));
    double VI = (nu / 120.0) * pow((cos(RadPHI)), 5.0) * (5.0 - (18.0 * pow((tan(RadPHI)), 2.0)) + pow((tan(RadPHI)), 4.0) + (14.0 * eta2) - (58.0 * pow((tan(RadPHI)), 2.0) * eta2));
    
    return e0 + (p * IV) + (pow(p, 3.0) * V) + (pow(p, 5.0) * VI);
    
}


double Lat_Long_to_North(double PHI, double LAM, double a, double b, double e0,  double n0, double f0, double PHI0, double LAM0)
{
/*
Function Lat_Long_to_North(PHI, LAM, a, b, e0, n0, f0, PHI0, LAM0)
'Project Latitude and longitude to Transverse Mercator northings
'Input: - _
 Latitude (PHI) and Longitude (LAM) in decimal degrees; _
 ellipsoid axis dimensions (a & b) in meters; _
 eastings (e0) and northings (n0) of false origin in meters; _
 central meridian scale factor (f0); _
 latitude (PHI0) and longitude (LAM0) of false origin in decimal degrees.

'REQUIRES THE "Marc" FUNCTION
*/
    //Convert angle measures to radians
    double Pi = 3.14159265358979;
    double RadPHI = PHI * (Pi / 180.0);
    double RadLAM = LAM * (Pi / 180.0);
    double RadPHI0 = PHI0 * (Pi / 180.0);
    double RadLAM0 = LAM0 * (Pi / 180.0);

    double af0 = a * f0;
    double bf0 = b * f0;
    double e2 = ((af0 * af0) - (bf0 * bf0)) / (af0 * af0);
    double n = (af0 - bf0) / (af0 + bf0);
    double nu = af0 / pow((1.0 - (e2 * pow((sin(RadPHI)), 2.0))),0.5);
    double rho = (nu * (1.0 - e2)) / (1.0 - pow(e2 * (sin(RadPHI)), 2.0));
    double eta2 = (nu / rho) - 1.0;
    double p = RadLAM - RadLAM0;
    double M = Marc(bf0, n, RadPHI0, RadPHI);
    
    double I = M + n0;
    double II = (nu / 2.0) * (sin(RadPHI)) * (cos(RadPHI));
    double III = ((nu / 24.0) * (sin(RadPHI)) * pow((cos(RadPHI)), 3.0)) * (5.0 - pow((tan(RadPHI)), 2.0) + (9.0 * eta2));
    double IIIA = ((nu / 720.0) * (sin(RadPHI)) * pow((cos(RadPHI)), 5.0)) * (61.0 - (58.0 * pow((tan(RadPHI)), 2.0)) + pow((tan(RadPHI)), 4.0));
    
    return I + (pow(p, 2.0) * II) + (pow(p, 4.0) * III) + (pow(p, 6.0) * IIIA);
   
}

double E_N_to_Lat(double East, double North, double a, double b, double e0, double n0, double f0, double PHI0, double LAM0)
{
/*Function E_N_to_Lat(East, North, a, b, e0, n0, f0, PHI0, LAM0)
'Un-project Transverse Mercator eastings and northings back to latitude.
'Input: - _
 eastings (East) and northings (North) in meters; _
 ellipsoid axis dimensions (a & b) in meters; _
 eastings (e0) and northings (n0) of false origin in meters; _
 central meridian scale factor (f0) and _
 latitude (PHI0) and longitude (LAM0) of false origin in decimal degrees.

'REQUIRES THE "Marc" AND "InitialLat" FUNCTIONS*/

//Convert angle measures to radians
    double Pi = 3.14159265358979;
    double RadPHI0 = PHI0 * (Pi / 180.0);
    //double RadLAM0 = LAM0 * (Pi / 180.0);

//Compute af0, bf0, e squared (e2), n and Et
    double af0 = a * f0;
    double bf0 = b * f0;
    double e2 = ((af0 * af0) - (bf0 * bf0)) / (af0 * af0);
    double n = (af0 - bf0) / (af0 + bf0);
    double Et = East - e0;

//Compute initial value for latitude (PHI) in radians
    double PHId = InitialLat(North, n0, af0, RadPHI0, n, bf0);
    
//Compute nu, rho and eta2 using value for PHId
    double nu = af0 / (sqrt(1.0 - (e2 * pow((sin(PHId)), 2.0))));
    double rho = (nu * (1.0 - e2)) / (1.0 - pow(e2 * (sin(PHId)), 2.0));
    double eta2 = (nu / rho) - 1.0;
    
//Compute Latitude
    double VII = (tan(PHId)) / (2.0 * rho * nu);
    double VIII = ((tan(PHId)) / (24.0 * rho * (pow(nu,3.0)))) * (5.0 + (3.0 * pow((tan(PHId)), 2.0)) + eta2 - (9.0 * eta2 * pow((tan(PHId)), 2.0)));
    double IX = ((tan(PHId)) / (720.0 * rho * (pow(nu , 5.0)))) * (61.0 + (90.0 * pow((tan(PHId)), 2.0)) + (45.0 * pow((tan(PHId)), 4.0)));
    
    return (180.0 / Pi) * (PHId - ((Et * Et) * VII) + (pow(Et , 4.0) * VIII) - (pow(Et , 6) * IX));

}


double E_N_to_Long(double East, double North, double a, double b, double e0, double n0, double f0, double PHI0, double LAM0)
{
/*'Un-project Transverse Mercator eastings and northings back to longitude.
'Input: - _
 eastings (East) and northings (North) in meters; _
 ellipsoid axis dimensions (a & b) in meters; _
 eastings (e0) and northings (n0) of false origin in meters; _
 central meridian scale factor (f0) and _
 latitude (PHI0) and longitude (LAM0) of false origin in decimal degrees.

'REQUIRES THE "Marc" AND "InitialLat" FUNCTIONS
*/

//Convert angle measures to radians
/*    Pi = 3.14159265358979
    RadPHI0 = PHI0 * (Pi / 180)
    RadLAM0 = LAM0 * (Pi / 180)*/
    double Pi = 3.14159265358979;
    double RadPHI0 = PHI0 * (Pi / 180.0);
    double RadLAM0 = LAM0 * (Pi / 180.0);

//Compute af0, bf0, e squared (e2), n and Et
/*    af0 = a * f0
    bf0 = b * f0
    e2 = ((af0 ^ 2) - (bf0 ^ 2)) / (af0 ^ 2)
    n = (af0 - bf0) / (af0 + bf0)
    Et = East - e0
*/
    double af0 = a * f0;
    double bf0 = b * f0;
    double e2 = ((af0 * af0) - (bf0 * bf0)) / (af0 * af0);
    double n = (af0 - bf0) / (af0 + bf0);
    double Et = East - e0;

//Compute initial value for latitude (PHI) in radians
    double PHId = InitialLat(North, n0, af0, RadPHI0, n, bf0);
    
//Compute nu, rho and eta2 using value for PHId
/*    nu = af0 / (Sqr(1 - (e2 * ((Sin(PHId)) ^ 2))))
    rho = (nu * (1 - e2)) / (1 - (e2 * (Sin(PHId)) ^ 2))
    eta2 = (nu / rho) - 1*/
    double nu = af0 / (sqrt(1.0 - (e2 * pow((sin(PHId)), 2.0))));
    double rho = (nu * (1.0 - e2)) / (1.0 - pow(e2 * (sin(PHId)), 2.0));
   //double eta2 = (nu / rho) - 1.0;
    
//Compute Longitude
    double X = pow((cos(PHId)), -1.0) / nu;
    double XI = (pow((cos(PHId)), -1.0) / (6.0 * pow(nu, 3))) * ((nu / rho) + (2.0 * pow((tan(PHId)), 2.0)));
    double XII = (pow((cos(PHId)), -1.0) / (120.0 * pow(nu, 5))) * (5.0 + (28.0 * pow((tan(PHId)), 2.0)) + (24.0 * pow((tan(PHId)), 4.0)));
    double XIIA = (pow((cos(PHId)), -1.0) / (5040.0 * pow(nu, 7))) * (61.0 + (662.0 * pow((tan(PHId)), 2.0)) + (1320.0 * pow((tan(PHId)), 4.0)) + (720.0 * pow((tan(PHId)), 6.0)));

    return (180.0 / Pi) * (RadLAM0 + (Et * X) - (pow(Et, 3.0) * XI) + (pow(Et, 5.0) * XII) - (pow(Et, 7.0) * XIIA));


}

double InitialLat(double North, double n0, double afo, double PHI0, double n, double bfo)
{
/*'Compute initial value for Latitude (PHI) IN RADIANS.
'Input: - _
 northing of point (North) and northing of false origin (n0) in meters; _
 semi major axis multiplied by central meridian scale factor (af0) in meters; _
 latitude of false origin (PHI0) IN RADIANS; _
 n (computed from a, b and f0) and _
 ellipsoid semi major axis multiplied by central meridian scale factor (bf0) in meters.
 
'REQUIRES THE "Marc" FUNCTION
'THIS FUNCTION IS CALLED BY THE "E_N_to_Lat", "E_N_to_Long" and "E_N_to_C" FUNCTIONS
'THIS FUNCTION IS ALSO USED ON IT'S OWN IN THE  "Projection and Transformation Calculations.xls" SPREADSHEET
*/

//First PHI value (PHI1)
    double PHI1 = ((North - n0) / afo) + PHI0;
//Calculate M
    double M = Marc(bfo, n, PHI0, PHI1);
    
//Calculate new PHI value (PHI2)
    double PHI2 = ((North - n0 - M) / afo) + PHI1;
    
//Iterate to get final value for InitialLat
    while( fabs(North - n0 - M) > 0.00001)
    {
        PHI2 = ((North - n0 - M) / afo) + PHI1;
        M = Marc(bfo, n, PHI0, PHI2);
        PHI1 = PHI2;
    }
    
    return PHI2;
    
}

double Marc(double bf0, double n, double PHI0, double PHI)
{
/*Compute meridional arc.
Input: - _
 ellipsoid semi major axis multiplied by central meridian scale factor (bf0) in meters; _
 n (computed from a, b and f0); _
 lat of false origin (PHI0) and initial or final latitude of point (PHI) IN RADIANS.

'THIS FUNCTION IS CALLED BY THE - _
 "Lat_Long_to_North" and "InitialLat" FUNCTIONS
'THIS FUNCTION IS ALSO USED ON IT'S OWN IN THE "Projection and Transformation Calculations.xls" SPREADSHEET
*/
/*    Marc = bf0 * (((1 + n + ((5 / 4) * (n ^ 2)) + ((5 / 4) * (n ^ 3))) * (PHI - PHI0)) _
    - (((3 * n) + (3 * (n ^ 2)) + ((21 / 8) * (n ^ 3))) * (Sin(PHI - PHI0)) * (Cos(PHI + PHI0))) _
    + ((((15 / 8) * (n ^ 2)) + ((15 / 8) * (n ^ 3))) * (Sin(2 * (PHI - PHI0))) * (Cos(2 * (PHI + PHI0)))) _
    - (((35 / 24) * (n ^ 3)) * (Sin(3 * (PHI - PHI0))) * (Cos(3 * (PHI + PHI0)))))*/

    return bf0 * (((1.0 + n + ((5.0 / 4.0) * pow(n, 2.0)) + ((5.0 / 4.0) * pow(n, 3.0))) * (PHI - PHI0))
    - (((3.0 * n) + (3.0 * pow(n, 2.0)) + ((21.0 / 8.0) * pow(n, 3.0))) * (sin(PHI - PHI0)) * (cos(PHI + PHI0)))
    + ((((15.0 / 8.0) * pow(n, 2.0)) + ((15.0 / 8.0) * pow(n, 3.0))) * (sin(2.0 * (PHI - PHI0))) * (cos(2.0 * (PHI + PHI0))))
    - (((35.0 / 24.0) * pow(n, 3.0)) * (sin(3.0 * (PHI - PHI0))) * (cos(3.0 * (PHI + PHI0)))));

}

/*
Function Lat_Long_to_C(PHI, LAM, LAM0, a, b, f0)
'Compute convergence (in decimal degrees) from latitude and longitude
'Input: - _
 latitude (PHI), longitude (LAM) and longitude (LAM0) of false origin in decimal degrees; _
 ellipsoid axis dimensions (a & b) in meters; _
 central meridian scale factor (f0).

'Convert angle measures to radians
    Pi = 3.14159265358979
    RadPHI = PHI * (Pi / 180)
    RadLAM = LAM * (Pi / 180)
    RadLAM0 = LAM0 * (Pi / 180)
        
'Compute af0, bf0 and e squared (e2)
    af0 = a * f0
    bf0 = b * f0
    e2 = ((af0 ^ 2) - (bf0 ^ 2)) / (af0 ^ 2)
    
'Compute nu, rho, eta2 and p
    nu = af0 / (Sqr(1 - (e2 * ((Sin(RadPHI)) ^ 2))))
    rho = (nu * (1 - e2)) / (1 - (e2 * (Sin(RadPHI)) ^ 2))
    eta2 = (nu / rho) - 1
    p = RadLAM - RadLAM0

'Compute Convergence
    XIII = Sin(RadPHI)
    XIV = ((Sin(RadPHI) * ((Cos(RadPHI)) ^ 2)) / 3) * (1 + (3 * eta2) + (2 * (eta2 ^ 2)))
    XV = ((Sin(RadPHI) * ((Cos(RadPHI)) ^ 4)) / 15) * (2 - ((Tan(RadPHI)) ^ 2))

    Lat_Long_to_C = (180 / Pi) * ((p * XIII) + ((p ^ 3) * XIV) + ((p ^ 5) * XV))

End Function

Function E_N_to_C(East, North, a, b, e0, n0, f0, PHI0)
'Compute convergence (in decimal degrees) from easting and northing
'Input: - _
 Eastings (East) and Northings (North) in meters; _
 ellipsoid axis dimensions (a & b) in meters; _
 easting (e0) and northing (n0) of true origin in meters; _
 central meridian scale factor (f0); _
 latitude of central meridian (PHI0) in decimal degrees.
 
'REQUIRES THE "Marc" AND "InitialLat" FUNCTIONS

'Convert angle measures to radians
    Pi = 3.14159265358979
    RadPHI0 = PHI0 * (Pi / 180)
        
'Compute af0, bf0, e squared (e2), n and Et
    af0 = a * f0
    bf0 = b * f0
    e2 = ((af0 ^ 2) - (bf0 ^ 2)) / (af0 ^ 2)
    n = (af0 - bf0) / (af0 + bf0)
    Et = East - e0
    
'Compute initial value for latitude (PHI) in radians
    PHId = InitialLat(North, n0, af0, RadPHI0, n, bf0)
    
'Compute nu, rho and eta2 using value for PHId
    nu = af0 / (Sqr(1 - (e2 * ((Sin(PHId)) ^ 2))))
    rho = (nu * (1 - e2)) / (1 - (e2 * (Sin(PHId)) ^ 2))
    eta2 = (nu / rho) - 1

'Compute Convergence
    XVI = (Tan(PHId)) / nu
    XVII = ((Tan(PHId)) / (3 * (nu ^ 3))) * (1 + ((Tan(PHId)) ^ 2) - eta2 - (2 * (eta2 ^ 2)))
    XVIII = ((Tan(PHId)) / (15 * (nu ^ 5))) * (2 + (5 * ((Tan(PHId)) ^ 2)) + (3 * ((Tan(PHId)) ^ 4)))
    
    E_N_to_C = (180 / Pi) * ((Et * XVI) - ((Et ^ 3) * XVII) + ((Et ^ 5) * XVIII))
   
End Function

Function Lat_Long_to_LSF(PHI, LAM, LAM0, a, b, f0)
'Compute local scale factor from latitude and longitude
'Input: - _
 latitude (PHI), longitude (LAM) and longitude (LAM0) of false origin in decimal degrees; _
 ellipsoid axis dimensions (a & b) in meters; _
 central meridian scale factor (f0).
 
 'Convert angle measures to radians
    Pi = 3.14159265358979
    RadPHI = PHI * (Pi / 180)
    RadLAM = LAM * (Pi / 180)
    RadLAM0 = LAM0 * (Pi / 180)
        
'Compute af0, bf0 and e squared (e2)
    af0 = a * f0
    bf0 = b * f0
    e2 = ((af0 ^ 2) - (bf0 ^ 2)) / (af0 ^ 2)
    
'Compute nu, rho, eta2 and p
    nu = af0 / (Sqr(1 - (e2 * ((Sin(RadPHI)) ^ 2))))
    rho = (nu * (1 - e2)) / (1 - (e2 * (Sin(RadPHI)) ^ 2))
    eta2 = (nu / rho) - 1
    p = RadLAM - RadLAM0

'Compute local scale factor
    XIX = ((Cos(RadPHI) ^ 2) / 2) * (1 + eta2)
    XX = ((Cos(RadPHI) ^ 4) / 24) * (5 - (4 * ((Tan(RadPHI)) ^ 2)) + (14 * eta2) - (28 * ((Tan(RadPHI * eta2)) ^ 2)))
    
    Lat_Long_to_LSF = f0 * (1 + ((p ^ 2) * XIX) + ((p ^ 4) * XX))
    
End Function

Function E_N_to_LSF(East, North, a, b, e0, n0, f0, PHI0)
'Compute local scale factor from from easting and northing
'Input: - _
 Eastings (East) and Northings (North) in meters; _
 ellipsoid axis dimensions (a & b) in meters; _
 easting (e0) and northing (n0) of true origin in meters; _
 central meridian scale factor (f0); _
 latitude of central meridian (PHI0) in decimal degrees.
 
'REQUIRES THE "Marc" AND "InitialLat" FUNCTIONS

'Convert angle measures to radians
    Pi = 3.14159265358979
    RadPHI0 = PHI0 * (Pi / 180)
        
'Compute af0, bf0, e squared (e2), n and Et
    af0 = a * f0
    bf0 = b * f0
    e2 = ((af0 ^ 2) - (bf0 ^ 2)) / (af0 ^ 2)
    n = (af0 - bf0) / (af0 + bf0)
    Et = East - e0
    
'Compute initial value for latitude (PHI) in radians
    PHId = InitialLat(North, n0, af0, RadPHI0, n, bf0)
    
'Compute nu, rho and eta2 using value for PHId
    nu = af0 / (Sqr(1 - (e2 * ((Sin(PHId)) ^ 2))))
    rho = (nu * (1 - e2)) / (1 - (e2 * (Sin(PHId)) ^ 2))
    eta2 = (nu / rho) - 1

'Compute local scale factor
    XXI = 1 / (2 * rho * nu)
    XXII = (1 + (4 * eta2)) / (24 * (rho ^ 2) * (nu ^ 2))
    
    E_N_to_LSF = f0 * (1 + ((Et ^ 2) * XXI) + ((Et ^ 4) * XXII))
    
End Function

Function E_N_to_t_minus_T(AtEast, AtNorth, ToEast, ToNorth, a, b, e0, n0, f0, PHI0)
'Compute (t-T) correction in decimal degrees at point (AtEast, AtNorth) to point (ToEast,ToNorth)
'Input: - _
 Eastings (AtEast) and Northings (AtNorth) in meters, of point where (t-T) is being computed; _
 Eastings (ToEast) and Northings (ToNorth) in meters, of point at other end of line to which (t-T) is being computed; _
 ellipsoid axis dimensions (a & b) and easting & northing (e0 & n0) of true origin in meters; _
 central meridian scale factor (f0); _
 latitude of central meridian (PHI0) in decimal degrees.

'REQUIRES THE "Marc" AND "InitialLat" FUNCTIONS

'Convert angle measures to radians
    Pi = 3.14159265358979
    RadPHI0 = PHI0 * (Pi / 180)
        
'Compute af0, bf0, e squared (e2), n and Nm (Northing of mid point)
    af0 = a * f0
    bf0 = b * f0
    e2 = ((af0 ^ 2) - (bf0 ^ 2)) / (af0 ^ 2)
    n = (af0 - bf0) / (af0 + bf0)
    Nm = (AtNorth + ToNorth) / 2
    
'Compute initial value for latitude (PHI) in radians
    PHId = InitialLat(Nm, n0, af0, RadPHI0, n, bf0)
    
'Compute nu, rho and eta2 using value for PHId
    nu = af0 / (Sqr(1 - (e2 * ((Sin(PHId)) ^ 2))))
    rho = (nu * (1 - e2)) / (1 - (e2 * (Sin(PHId)) ^ 2))
    
'Compute (t-T)
    XXIII = 1 / (6 * nu * rho)
    
    E_N_to_t_minus_T = (180 / Pi) * ((2 * (AtEast - e0)) + (ToEast - e0)) * (AtNorth - ToNorth) * XXIII

End Function

Function TrueAzimuth(AtEast, AtNorth, ToEast, ToNorth, a, b, e0, n0, f0, PHI0)
'Compute true azimuth in decimal degrees at point (AtEast, AtNorth) to point (ToEast,ToNorth)
'Input: - _
 Eastings (AtEast) and Northings (AtNorth) in meters, of point where true azimuth is being computed; _
 Eastings (ToEast) and Northings (ToNorth) in meters, of point at other end of line to which true azimuth is being computed; _
 ellipsoid axis dimensions (a & b) and easting & northing (e0 & n0) of true origin in meters; _
 central meridian scale factor (f0); _
 latitude of central meridian (PHI0) in decimal degrees.

'REQUIRES THE "Marc", "InitialLat", "E_N_to_t_minus_T" and "E_N_to_C" FUNCTIONS
  
'Compute eastings and northings differences
    Diffe = ToEast - AtEast
    Diffn = ToNorth - AtNorth

'Compute grid bearing
    If Diffe = 0 Then
        If Diffn < 0 Then
            GridBearing = 180
        Else
            GridBearing = 0
        End If
        GoTo EndOfComputeBearing
    End If
    
    Ratio = Diffn / Diffe
    Pi = 3.14159265358979
    GridAngle = (180 / Pi) * Atn(Ratio)
    
    If Diffe > 0 Then
        GridBearing = 90 - GridAngle
    End If
    
    If Diffe < 0 Then
        GridBearing = 270 - GridAngle
    End If
EndOfComputeBearing:

'Compute convergence
    Convergence = E_N_to_C(AtEast, AtNorth, a, b, e0, n0, f0, PHI0)
    
'Compute (t-T) correction
    t_minus_T = E_N_to_t_minus_T(AtEast, AtNorth, ToEast, ToNorth, a, b, e0, n0, f0, PHI0)

'Compute initial azimuth
    InitAzimuth = GridBearing + Convergence - t_minus_T
    
'Set TrueAzimuth >=0 and <=360
    If InitAzimuth < 0 Then
        TrueAzimuth = InitAzimuth + 360
    ElseIf InitAzimuth > 360 Then
        TrueAzimuth = InitAzimuth - 360
    Else
        TrueAzimuth = InitAzimuth
    End If

End Function*/

void ConvertCasToWgs84(double ea, double no, double he,
	double &latOut, double &lonOut, double &heOut)
{
        const double Pi = 3.14159265358979;
        const double unit = 1609.347955248; // Metres in a mile
        const long double a = ( 6377563.3963534083 / unit );
        const long double b = ( 6356256.909589071 / unit );
	const long double e2 = ((a*a)-(b*b))/(a*a);
        const int FE = 0;
        const int FN = 0;
	const int F0 = 1;

        // Lat and lon of origin in Delamere Forest, Cheshire
        const long double Lat0 = 53.221465 * (Pi / 180.0 );
        const long double Lon0 = -2.68432278 * (Pi / 180.0 );

	//const long double QQ = ( 1 - ( e2 / 4 ) - ( ( 3 / 64 ) * pow( e2, 2 ) ) - ( ( 5 / 256 ) * pow( e2, 3 ) ) );
	const long double N = ( ( a - b ) / ( a + b ) );

	long double lat_ = ((no - FN) / (a * F0)) + Lat0;
    	for( bool iter_complete = false; !iter_complete; ) {
        	long double M = calc_M(lat_ - Lat0, lat_ + Lat0, N, b, 1);
        	if (fabs(no - FN - M) >= 0.00000001) { lat_ = (((no - FN - M) / (a * F0)) + lat_); }
          	else { iter_complete = true; }
    	}
	//long double M0 = calc_M( Lat0, Lon0, N, b, 1 );
	//long double M1 = M0 + no - FN;
	//long double u1 = M1 / ( a * QQ );
	//long double e1 = (1-(sqrt(1-e2))) / (1+(sqrt(1-e2)));
	long double w1 = lat_;
    	//long double w1a = ((3*e1/2) - (27*e1*e1*e1/32)) * sin(2*u1);
    	//long double w1b = ((21*e1*e1/16) - (55*e1*e1*e1*e1/32)) * sin(4*u1);
    	//long double w1c = (151*e1*e1*e1/96) * sin(6*u1);
    	//long double w1d = (1097*e1*e1*e1*e1/512) * sin(8*u1);
    	//long double w1 = u1+w1a+w1b+w1c+w1d;
    	long double T1 = pow(tan(w1), 2);
    	long double v1 = a / sqrt(1 - (e2 * pow(sin(w1), 2)));
    	long double r1 = (a * (1 - e2)) / pow((1 - (e2 * pow(sin(w1), 2))), 1.5);
    	long double D = (ea - FE) / v1;
    	long double KK = ((1 + (3 * T1)) * (pow(D, 4) / 24));
    	long double LL = (v1 * tan(w1)) / r1;
    	long double MM = w1 - (LL * ((pow(D, 2) / 2) - KK));
    	long double RR = (T1 * (pow(D, 3) / 3));
    	long double SS = ((1 + (3 * T1)) * T1 * (pow(D, 5) / 15));
    	long double TT = ((D - RR + SS) / cos(w1));

	double gboslat = MM * ( 180 / Pi );
	double gboslng = ( Lon0 + TT ) * ( 180 / Pi );

	ConvertGbos1936LatLngToWgs84(gboslat, gboslng, he, latOut, lonOut, heOut); 
}

void ConvertBnToWgs84( double orglat, double orglon, double ea, double no, double he,
	double &latOut, double &lonOut, double &heOut)
{
	const double Pi = 3.14159265358979;
        const double unit = 1609.347955248; // Metres in a mile
        const double a = (6377563.396353409 / unit );
        const double b = (6356256.909589071 / unit );
        const int FE = 0;
        const int FN = 0;
        const double Lat0 = orglat * (Pi / 180.0);
        const double Lon0 = orglon * (Pi / 180.0);
        const double e2 = ((a * a) - (b * b)) / (a * a);

	double X = ea - FE;
        double Y = no - FN;
        double h0 = (cos(Lat0) / sqrt(1 - (e2 * pow(sin(Lat0), 2))));
        double QQ = (1 - (e2 / 4) - ((3 * pow(e2, 2)) / 64) - ((5 * pow(e2, 3)) / 256));
        double RR = (((3 * e2) / 8) + ((3 * pow(e2, 2)) / 32) + ((45 * pow(e2, 3)) / 1024));
        double SS = (((15 * pow(e2, 2)) / 256) + ((45 * pow(e2, 3)) / 1024));
        double TT = (((35 * pow(e2, 3)) / 3072));
        double M0 = (a * ((QQ * Lat0) - (RR * (sin(2 * Lat0))) + (SS * (sin(4 * Lat0))) - (TT * (sin(6 * Lat0)))));

        double p = sqrt(pow(X, 2) + pow((((a * h0) / sin(Lat0)) - Y), 2));
        if (Lat0 >= 0) {
                p = fabs(p);
	}
        if (Lat0 < 0) {
                p = fabs(p) * -1;
	}
        double M = ((a * h0) / sin(Lat0)) + M0 - p;
        double u = M / (a * (1 - (e2 / 4) - ((3 * pow(e2, 2)) / 64) - ((5 * pow(e2, 3)) / 256)));
        double FF = (1 - sqrt(1 - e2));
        double GG = (1 + sqrt(1 - e2));
        double e1 = (FF / GG);
        double inter1 = ((((3 * e1) / 2) - ((27 * pow(e1, 3)) / 32)) * sin(2 * u));
        double inter2 = ((((21 * pow(e1, 2)) / 16) - ((55 * pow(e1, 4)) / 32)) * sin(4 * u));
        double inter3 = ((((151 * pow(e1, 3)) / 96)) * sin(6 * u));
        double inter4 = ((((1097 * pow(e1, 4)) / 512)) * sin(8 * u));


        double lat = (u + inter1 + inter2 + inter3 + inter4);
	double lon = 0.0;
	if (fabs((180/ Pi)* lat) < 89.99999999 || fabs((180/Pi)*lat) > 90.00000001) {
        	double h = cos(lat) / sqrt(1 - (e2 * pow(sin(lat), 2)));
        	double QQ = Lon0 + (p * (atan(X / (((a * h0) / sin(Lat0)) - Y))) / (a * h));
        	double RR = Lon0 + (p * (atan((X * -1) / (Y - ((a * h0) / sin(Lat0))))) / (a * h));
        	if (Lat0 >= 0) { lon = QQ; }
        	if (Lat0 < 0) { lon = RR; }
    	} else { 
		lon = Lon0; 
	}

	double gboslat = lat * ( 180 / Pi );
	double gboslng = lon * ( 180 / Pi );
	ConvertGbos1936LatLngToWgs84(gboslat, gboslng, he, latOut, lonOut, heOut); 
}

void ConvertGbos1936ToWgs84(double ea, double no, double he,
	double &latOut, double &lonOut, double &heOut)
{
	double gbos36lat = E_N_to_Lat(ea,no, 6377563.396, 6356256.910, 400000, -100000,0.999601272, 49.00000,-2.00000);
	double gbos36lon = E_N_to_Long(ea,no, 6377563.396, 6356256.910, 400000, -100000,0.999601272, 49.00000,-2.00000);

	ConvertGbos1936LatLngToWgs84( gbos36lat, gbos36lon, he, latOut, lonOut, heOut );
}

void ConvertOsiToWgs84(double ea, double no, double he,
	double &latOut, double &lonOut, double &heOut)
{
  const double Pi = 3.14159265358979;
  double a = 6377340.189446778;
  double b = 6356034.448383377;
  double n = ((a-b)/(a+b));
  double e2 = 0.006670540000123429;
  double A1 = a/(1+n)*(n*n*(n*n*((n*n)+4)+64)+256)/256;
  double h1 = n*(n*(n*(n*(n*(384796*n-382725)-6720)+932400)-1612800)+1209600)/2419200;
  double h2 = n*n*(n*(n*((1695744-1118711*n)*n-1174656)+258048)+80640)/3870720;
  double h3 = n*n*n*(n*(n*(22276*n-16929)-15984)+12852)/362880;
  double h4 = n*n*n*n*((-830251*n-158400)*n+197865)/7257600;
  double h5 = (453717-435388*n)*n*n*n*n*n/15966720;
  double h6 = 20648693*n*n*n*n*n*n/638668800;
  double M = calc_M(0.9337511498169663,0.9337511498169663,n,b,1.000035);
  double E = (no-250000+M)/(A1*1.000035);
  double nn = (ea-200000)/(A1*1.000035);
  double E1i = h1*sin(2*E)*cosh(2*nn);
  double E2i = h2*sin(4*E)*cosh(4*nn);
  double E3i = h3*sin(6*E)*cosh(6*nn);
  double E4i = h4*sin(8*E)*cosh(8*nn);
  double E5i = h5*sin(10*E)*cosh(10*nn);
  double E6i = h6*sin(12*E)*cosh(12*nn);
  double n1i = h1*cos(2*E)*sinh(2*nn);
  double n2i = h2*cos(4*E)*sinh(4*nn);
  double n3i = h3*cos(6*E)*sinh(6*nn);
  double n4i = h4*cos(8*E)*sinh(8*nn);
  double n5i = h5*cos(10*E)*sinh(10*nn);
  double n6i = h6*cos(12*E)*sinh(12*nn);
  double Ei = E-(E1i+E2i+E3i+E4i+E5i+E6i);
  double ni = nn-(n1i+n2i+n3i+n4i+n5i+n6i);
  double B = asin((1/cosh(ni))*sin(Ei));
  double l = asin(tanh(ni)/cos(B));
  double Q = asinh(tan(B));
  double Qi = Q+(sqrt(e2)*atanh(sqrt(e2)*tanh(Q)));
  for(;;) {
    double newv = Q+(sqrt(e2)*atanh(sqrt(e2)*tanh(Qi)));
    if (abs(Qi-newv) < 1e-11) {
      break;
    }
    Qi = newv; 
  }
  double lati = (atan(sinh(Qi)));
  double loni = (-0.13962634015954636+l );

  double r_latOut = 0;
  double r_lonOut = 0;

  ConvertOsi65ToWgs84( lati, loni, r_latOut, r_lonOut );

  latOut = r_latOut * ( 180 / Pi );
  lonOut = r_lonOut * ( 180 / Pi );

  heOut = 0;
}

void ConvertGbos1936LatLngToWgs84(double gbos36lat, double gbos36lon, double he,
	double &latOut, double &lonOut, double &heOut)
{
	double gbos36x = Lat_Long_H_to_X(gbos36lat,gbos36lon,0.0,6377563.396,6356256.910);
	double gbos36y = Lat_Long_H_to_Y(gbos36lat,gbos36lon,0.0,6377563.396,6356256.910);
	double gbos36z = Lat_H_to_Z(gbos36lat,gbos36lon,he,6377563.396,6356256.910);

	double wgs84x = Helmert_X(gbos36x, gbos36y, gbos36z, 446.448, 0.2470, 0.8421, -20.4894);
	double wgs84y = Helmert_Y(gbos36x, gbos36y, gbos36z, -125.157, 0.1502, 0.8421, -20.4894);
	double wgs84z = Helmert_Z(gbos36x, gbos36y, gbos36z, 542.060,0.1502,0.2470,-20.4894);

	latOut = XYZ_to_Lat(wgs84x, wgs84y, wgs84z,6378137.000,6356752.314);
	lonOut = XYZ_to_Long(wgs84x, wgs84y);
	heOut = XYZ_to_H(wgs84x, wgs84y, wgs84z,6378137.000,6356752.314);
}

void ConvertWgs84ToGbos1936(double lat, double lon, double he,
	double &eaOut, double &noOut, double &heOut)
{
	double gbos36lat2 = 0.0;
	double gbos36lon2 = 0.0;
	ConvertWgs84ToGbos1936LatLng(lat, lon, he, gbos36lat2, gbos36lon2 );
	eaOut = Lat_Long_to_East(gbos36lat2, gbos36lon2, 6377563.396, 6356256.910, 400000,0.999601272, 49.00000,-2.00000);
	noOut = Lat_Long_to_North(gbos36lat2, gbos36lon2, 6377563.396, 6356256.910, 400000, -100000,0.999601272, 49.00000,-2.00000);
	heOut = 0;

	//cout << eaOut << "," << noOut << endl;
}

void ConvertWgs84ToOsi(double lat, double lon, double he,
	double &eaOut, double &noOut, double &heOut)
{
  const double Pi = 3.14159265358979;
  double r_lat = 0;
  double r_lon = 0;
  ConvertWgs84ToOsi65( lat * ( Pi / 180 ), lon * ( Pi / 180 ), r_lat, r_lon );
  double a = 6377340.189446778;
  double b = 6356034.448383377;
  double n = ((a-b)/(a+b));
  double e2 = 0.006670540000123429;
  double ei2 = e2/(1-e2);
  double A1 = a/(1+n)*(n*n*(n*n*((n*n)+4)+64)+256)/256;
  double h1i = n*(n*(n*(n*(n*(31564*n-66675)+34440)+47250)-100800)+75600)/151200;
  double h2i = n*n*(n*(n*((863232-1983433*n)*n+748608)-1161216)+524160)/1935360;
  double h3i = n*n*n*(n*(n*(670412*n+406647)-533952)+184464)/725760;
  double h4i = n*n*n*n*(n*(6601661*n-7732800)+2230245)/7257600;
  double h5i = (3438171-13675556*n)*n*n*n*n*n/7983360;
  double h6i = 212378941*n*n*n*n*n*n/319334400;
  double Qi = asinh(tan(r_lat));
  double Qii = atanh(sqrt(e2)*sin(r_lat));
  double Q = Qi-(sqrt(e2)*Qii);
  double l = r_lon - (-0.13962634015954636);
  double BB = atan(sinh(Q));
  double ni = atanh(cos(BB)*sin(l));
  double Ei = asin(sin(BB)/(1/cosh(ni)));
  double E1 = h1i*sin(2*Ei)*cosh(2*ni);
  double E2 = h2i*sin(4*Ei)*cosh(4*ni);
  double E3 = h3i*sin(6*Ei)*cosh(6*ni);
  double E4 = h4i*sin(8*Ei)*cosh(8*ni);
  double E5 = h5i*sin(10*Ei)*cosh(10*ni);
  double E6 = h6i*sin(12*Ei)*cosh(12*ni);
  double n1 = h1i*cos(2*Ei)*sinh(2*ni);
  double n2 = h2i*cos(4*Ei)*sinh(4*ni);
  double n3 = h3i*cos(6*Ei)*sinh(6*ni);
  double n4 = h4i*cos(8*Ei)*sinh(8*ni);
  double n5 = h5i*cos(10*Ei)*sinh(10*ni);
  double n6 = h6i*cos(12*Ei)*sinh(12*ni);
  double E = Ei+E1+E2+E3+E4+E5+E6;
  double nn = ni+n1+n2+n3+n4+n5+n6;
  double M = calc_M(0.9337511498169663,0.9337511498169663,n,b,1.000035);
  noOut = (A1*E*1.000035+250000-M);
  eaOut = (A1*nn*1.000035)+200000;
  heOut = 0;

  //cout << eaOut << "," << noOut << endl;
}

void ConvertWgs84ToCas(double wlat, double wlon, double he,
	double &eaOut, double &noOut )
{
        const double Pi = 3.14159265358979;
        const double unit = 1609.347955248; // Metres in a mile
        const double a = (6377563.396353409 / unit );
        const double b = (6356256.909589071 / unit );
        const int FE = 0;
        const int FN = 0;

        // Lat and lon of origin in Delamere Forest, Cheshire
        const long double Lat0 = 53.221465 * (Pi / 180.0 );
        const long double Lon0 = -2.68432278 * (Pi / 180.0 );

        const long double e2 = ((a*a)-(b*b))/(a*a);

        double gbos36lat2 = 0.0;
        double gbos36lon2 = 0.0;
        ConvertWgs84ToGbos1936LatLng(wlat, wlon, he, gbos36lat2, gbos36lon2 );

        long double lat = gbos36lat2 * (Pi / 180.0);
        long double lon = gbos36lon2 * (Pi / 180.0);

        long double v = a / sqrt((1 - (e2 * pow(sin(lat), 2))));
        long double T = pow(tan(lat), 2);
        long double AA = ((lon - Lon0) * cos(lat));
        long double C = ((e2 * pow(cos(lat), 2) / (1 - e2)));
        long double GG = (AA - (T * pow(AA, 3) / 6) - ((8 - T + (8 * C)) * T * pow(AA, 5) / 120));
        long double N = ((a - b) / (a + b));

        long double M0 = calc_M(Lat0, Lat0, N, b, 1);
        long double M = calc_M(lat, lat, N, b, 1);
        long double FF = ((pow(AA, 2) / 2) + ((5 - T + (6 * C)) * pow(AA, 4) / 24));

        eaOut = FE + (v * GG);
        noOut = FN + M - M0 + (v * tan(lat) * FF);
}

void ConvertWgs84ToBn(const double orglat, const double orglon, 
	double wlat, double wlon, double he,
        double &eaOut, double &noOut )
{
        const double Pi = 3.14159265358979;
        const double unit = 1609.347955248; // Metres in a mile
        const double a = (6377563.396353409 / unit );
        const double b = (6356256.909589071 / unit );
        const int FE = 0;
        const int FN = 0;
        const double Lat0 = 57.5 * (Pi / 180.0);
	const double Lon0 = -4 * (Pi / 180.0);
    	const double e2 = ((a * a) - (b * b)) / (a * a);

        double gbos36lat2 = 0.0;
        double gbos36lon2 = 0.0;
        ConvertWgs84ToGbos1936LatLng(wlat, wlon, he, gbos36lat2, gbos36lon2 );

        double lat = gbos36lat2 * (Pi / 180.0);
        double lon = gbos36lon2 * (Pi / 180.0);

        double h = (cos(lat) / sqrt(1 - (e2 * pow(sin(lat), 2))));
        double h0 = (cos(Lat0) / sqrt(1 - (e2 * pow(sin(Lat0), 2))));
        double QQ = (1 - (e2 / 4) - ((3 * pow(e2, 2)) / 64) - ((5 * pow(e2, 3)) / 256));
        double RR = (((3 * e2) / 8) + ((3 * pow(e2, 2)) / 32) + ((45 * pow(e2, 3)) / 1024));
        double SS = (((15 * pow(e2, 2)) / 256) + ((45 * pow(e2, 3)) / 1024));
        double TT = (((35 * pow(e2, 3)) / 3072));
        double M0 = (a * ((QQ * Lat0) - (RR * (sin(2 * Lat0))) + (SS * (sin(4 * Lat0))) - (TT * (sin(6 * Lat0)))));
        double M = (a * ((QQ * lat) - (RR * (sin(2 * lat))) + (SS * (sin(4 * lat))) - (TT * (sin(6 * lat)))));
        double p = (((a * h0) / sin(Lat0)) + M0 - M);
        double T = ((a * h * (lon - Lon0)) / p);
        double X = ((a * h0) / sin(Lat0)) - (p * cos(T)) + FN;

        eaOut = ( p * sin(T) ) + FE;
        noOut = X;
}

double calc_M(double latdiff, double latsum, double nn, double eb, double F0)
{
	double n2 = pow(nn,2);
	double n3 = pow(nn,3);
	double BB = (1+nn+((5/4)*n2)+((5/4)*n3))*latdiff;
    	double CC = ((3*nn)+(3*n2)+((21/8)*n3))*sin(latdiff)*cos(latsum);
    	double DD = (((15/8)*n2)+((15/8)*n3))*sin(2*latdiff)*cos(2*latsum);
	double EE = ((35/24)*n3)*sin(3*latdiff)*cos(3*latsum);

	return (eb*F0*(BB-CC+DD-EE));
}

void ConvertWgs84ToGbos1936LatLng(double lat, double lon, double he,
	double &gbos36lat2, double &gbos36lon2 )
{
	double wgs84x2 = Lat_Long_H_to_X(lat,lon,0.0,6378137.000,6356752.314);
	double wgs84y2 = Lat_Long_H_to_Y(lat,lon,0.0,6378137.000,6356752.314);
	double wgs84z2 = Lat_H_to_Z(lat,lon,he,6378137.000,6356752.314);

	//cout << wgs84x2 << "," << wgs84y2 << "," << wgs84z2 << endl;

	double gbos36x2 = Helmert_X(wgs84x2, wgs84y2, wgs84z2, -446.448, -0.2470, -0.8421, 20.4894);
	double gbos36y2 = Helmert_Y(wgs84x2, wgs84y2, wgs84z2, 125.157, -0.1502, -0.8421, 20.4894);
	double gbos36z2 = Helmert_Z(wgs84x2, wgs84y2, wgs84z2, -542.060,-0.1502,-0.2470,20.4894);

	//cout << gbos36x2 << "," << gbos36y2 << "," << gbos36z2 << endl;

	gbos36lat2 = XYZ_to_Lat(gbos36x2, gbos36y2, gbos36z2,6377563.396,6356256.910);
	gbos36lon2 = XYZ_to_Long(gbos36x2, gbos36y2);
	//double gbos36h2 = XYZ_to_H(gbos36x2, gbos36y2, gbos36z2,6377563.396,6356256.910);

	//cout << eaOut << "," << noOut << endl;
}

int TestGbos1936()
{
	double easting = 330000.0;
	double northing = 550000.0;
	double height = 0.0;
	cout << easting << "," << northing << endl;

	//double E_N_to_Lat(double East, double North, double a, double b, double e0, double n0, double f0, double PHI0, double LAM0)
	double gbos36lat = E_N_to_Lat(easting,northing, 6377563.396, 6356256.910, 400000, -100000,0.999601272, 49.00000,-2.00000);
	//double E_N_to_Long(double East, double North, double a, double b, double e0, double n0, double f0, double PHI0, double LAM0)
	double gbos36lon = E_N_to_Long(easting,northing, 6377563.396, 6356256.910, 400000, -100000,0.999601272, 49.00000,-2.00000);
	cout <<gbos36lat << "," << gbos36lon << endl;

	//gbos36lat=54.840237696502;
	//gbos36lon=-3.043369082975;

	double gbos36x = Lat_Long_H_to_X(gbos36lat,gbos36lon,0.0,6377563.396,6356256.910);
	double gbos36y = Lat_Long_H_to_Y(gbos36lat,gbos36lon,0.0,6377563.396,6356256.910);
	double gbos36z = Lat_H_to_Z(gbos36lat,gbos36lon,height,6377563.396,6356256.910);

	cout << gbos36x << "," << gbos36y << "," << gbos36z << endl;

	double wgs84x = Helmert_X(gbos36x, gbos36y, gbos36z, 446.448, 0.2470, 0.8421, -20.4894);
	double wgs84y = Helmert_Y(gbos36x, gbos36y, gbos36z, -125.157, 0.1502, 0.8421, -20.4894);
	double wgs84z = Helmert_Z(gbos36x, gbos36y, gbos36z, 542.060,0.1502,0.2470,-20.4894);
	cout << wgs84x << "," << wgs84y << "," << wgs84z << endl;

	double wgs84lat = XYZ_to_Lat(wgs84x, wgs84y, wgs84z,6378137.000,6356752.314);
	double wgs84lon = XYZ_to_Long(wgs84x, wgs84y);
	double wgs84h = XYZ_to_H(wgs84x, wgs84y, wgs84z,6378137.000,6356752.314);

	cout << wgs84lat << "," << wgs84lon << "," << wgs84h << endl;

	double wgs84x2 = Lat_Long_H_to_X(wgs84lat,wgs84lon,0.0,6378137.000,6356752.314);
	double wgs84y2 = Lat_Long_H_to_Y(wgs84lat,wgs84lon,0.0,6378137.000,6356752.314);
	double wgs84z2 = Lat_H_to_Z(wgs84lat,wgs84lon,wgs84h,6378137.000,6356752.314);

	cout << wgs84x2 << "," << wgs84y2 << "," << wgs84z2 << endl;

	double gbos36x2 = Helmert_X(wgs84x2, wgs84y2, wgs84z2, -446.448, -0.2470, -0.8421, 20.4894);
	double gbos36y2 = Helmert_Y(wgs84x2, wgs84y2, wgs84z2, 125.157, -0.1502, -0.8421, 20.4894);
	double gbos36z2 = Helmert_Z(wgs84x2, wgs84y2, wgs84z2, -542.060,-0.1502,-0.2470,20.4894);

	cout << gbos36x2 << "," << gbos36y2 << "," << gbos36z2 << endl;

	double gbos36lat2 = XYZ_to_Lat(gbos36x2, gbos36y2, gbos36z2,6377563.396,6356256.910);
	double gbos36lon2 = XYZ_to_Long(gbos36x2, gbos36y2);
	double gbos36h2 = XYZ_to_H(gbos36x2, gbos36y2, gbos36z2,6377563.396,6356256.910);	

	cout <<gbos36lat2 << "," << gbos36lon2 << "," << gbos36h2 << endl;

	double easting2 = Lat_Long_to_East(gbos36lat2, gbos36lon2, 6377563.396, 6356256.910, 400000,0.999601272, 49.00000,-2.00000);
	double northing2 = Lat_Long_to_North(gbos36lat2, gbos36lon2, 6377563.396, 6356256.910, 400000, -100000,0.999601272, 49.00000,-2.00000);
	
	cout << easting2 << "," << northing2 << endl;

	return 1;
}

void ConvertOsi65ToWgs84(double osilat, double osilon, double &latOut,
 double &lonOut )
{
  double latShift = 0;
  double lonShift = 0;
  GetOsiShift( osilat, osilon, latShift, lonShift );
  latOut = osilat + latShift;
  lonOut = osilon + lonShift;
}

void ConvertWgs84ToOsi65(double lat, double lon, double &latOut, double &lonOut )
{
   double old_lat = 0;
   double old_lon = 0;
   double shift_lat = 0;
   double shift_lon = 0;
   GetOsiShift( lat, lon, shift_lat, shift_lon );
   latOut = lat-shift_lat;
   lonOut = lon-shift_lon;
   for( ;; ) {
      old_lat = shift_lat;
      old_lon = shift_lon;
      GetOsiShift( lat, lon, shift_lat, shift_lon );
      if (abs(shift_lon-old_lon<1e-10) &&
        abs(shift_lat-old_lat<1e-10)) {
        break;
      } else {
         latOut=lat-shift_lat;
         lonOut=lat-shift_lon;
      }
   } 
}

void GetOsiShift(double lat, double lon, double &latOut, double &lonOut ) 
{
  const double Pi = 3.14159265358979;
  const double osiA[4][4] =
  {
    { 0.763, 0.123, 0.183, -0.374 },
    { -4.487, -0.515, 0.414, 13.11 },
    { 0.215, -0.57, 5.703, 113.743 },
    { -0.265, 2.852, -61.678, -265.898 }
  };

  const double osiB[4][4] =
  {
    { -2.81, -4.68, 0.17, 2.163 },
    { -0.341, -0.119, 3.913, 18.867 },
    { 1.196, 4.877, -27.795, -284.294 },
    { -0.887, -46.666, -95.377, -853.95 }
  };

  const double osik0 = 0.1;
  const double osilatm = 53.5;
  const double osilonm = -7.7;
  double n_U=osik0*((lat * ( 180 / Pi ))-osilatm);
  double n_V=osik0*((lon * ( 180 / Pi ))-osilonm);
  double dlat=0;
  double dlon=0;
  double coeff=1;
  for(int xx=0;xx<=3;xx++) {
    for(int yy=0;yy<=3;yy++) {
      coeff=pow(n_U,xx)*pow(n_V,yy);
      dlat+=(osiA[xx][yy]*coeff);
      dlon+=(osiB[xx][yy]*coeff);
    }
  }
  latOut = (dlat/3600) * ( Pi / 180 );
  lonOut = (dlon/3600) * ( Pi / 180 );
}


//***************************************************

void HelmertConverter::ConvertGbos1936ToWgs84(double ea, double no, double he,
	double &latOut, double &lonOut, double &heOut)
{
	::ConvertGbos1936ToWgs84(ea, no, he,
		latOut, lonOut, heOut);
}

void HelmertConverter::ConvertOsiToWgs84(double ea, double no, double he,
	double &latOut, double &lonOut, double &heOut)
{
	::ConvertOsiToWgs84(ea, no, he,
		latOut, lonOut, heOut);
}

void HelmertConverter::ConvertWgs84ToGbos1936(double lat, double lon, double he,
		double &eaOut, double &noOut, double &heOut)
{
	::ConvertWgs84ToGbos1936(lat, lon, he,
		eaOut, noOut, heOut);
}

void HelmertConverter::ConvertWgs84ToOsi(double lat, double lon, double he,
		double &eaOut, double &noOut, double &heOut)
{
	::ConvertWgs84ToOsi(lat, lon, he,
		eaOut, noOut, heOut);
}

void HelmertConverter::ConvertCasToWgs84(double lat, double lon, double he,
		double &eaOut, double &noOut, double &heOut )
{
	::ConvertCasToWgs84(lat, lon, he, eaOut, noOut, heOut );
}

void HelmertConverter::ConvertBnSToWgs84(double lat, double lon, double he,
		double &eaOut, double &noOut, double &heOut )
{
	::ConvertBnToWgs84(57.5, -4, lat, lon, he, eaOut, noOut, heOut );
}

void HelmertConverter::ConvertBnIToWgs84(double lat, double lon, double he,
		double &eaOut, double &noOut, double &heOut )
{
	::ConvertBnToWgs84(53.5, -8, lat, lon, he, eaOut, noOut, heOut );
}

void HelmertConverter::ConvertWgs84ToCas(double lat, double lon, double he,
		double &eaOut, double &noOut)
{
	::ConvertWgs84ToCas(lat, lon, he, eaOut, noOut);
}

void HelmertConverter::ConvertWgs84ToBnS(double lat, double lon, double he,
		double &eaOut, double &noOut)
{
	::ConvertWgs84ToBn(57.5, -4, lat, lon, he, eaOut, noOut);
}

void HelmertConverter::ConvertWgs84ToBnI(double lat, double lon, double he,
		double &eaOut, double &noOut)
{
	::ConvertWgs84ToBn(53.5, -8, lat, lon, he, eaOut, noOut);
}
