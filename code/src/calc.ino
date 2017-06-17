#include <math.h>

/*
    Equation of Time calculation
    *** No guarantees are implied. Use at your own risk ***

    Written by Del Smith, 2016-11-29

    Based on "Equation of time" WikiPedia article as of 2016-11-28
    (which describes angles in a bewildering mixture of degrees and radians)

    It appears to give a good result, but I make no claims for accuracy.
*/

// constants  (all angles as radians)
#define OMEGA       ( 2.0 * M_PI / 365.25 )
#define LAMBDA      ( 23.44 * M_PI / 180.0 )

// calculate the Equation of Time from 'day of year'
double equationOfTime(int day) {

    // the result is in seconds
    // add to the Mean Solar Time  (UTC + Longitude*240 sec.) to get Apparent Solar Time
    // D                                                          day of the year
    // A = W*(D+10)                                               angle of revolution (circular)
    // B = A + 0.0333*sin(W*(D-2))                                angle of revolution (elliptical)
    // C = ( A - atan[  tan(B) / cos (inclination) ] ) / 'pi'     angular correction
    // EOT = 43200 × (C − int(C+0.5))                             Equation of Time in seconds

    double alpha = OMEGA * ( ( day + 10 ) % 365 );                          // angle in (mean) circular orbit, solar year starts 21. Dec
    double beta = alpha + 0.0333 * sin( OMEGA * ( ( day - 2 ) % 365 ) );    // angle in elliptical orbit, from perigee 3. Jan (radians)
    double gamma = ( alpha - atan( tan(beta) / cos(LAMBDA))) / M_PI;        // angular correction
    return ( 43200.0 * ( gamma - (int) ( gamma + 0.5 ) ) );                 // equation of time in seconds

}
