/* Generated by Together */

#include "SectorZone.hpp"
#include "Navigation/Geometry/GeoVector.hpp"

GEOPOINT SectorZone::get_boundary_parametric(double t) const
{ 
  AIRCRAFT_STATE state;
  state.Location = GeoVector(Radius*0.999, t*360).end_point(getLocation());

  if (isInSector(state)) {
    return state.Location;
  } else {
    return getLocation();
  }
}

double SectorZone::score_adjustment()
{
  return 0.0;
}

void 
SectorZone::updateSector() 
{
  SectorStart = GeoVector(Radius, StartRadial).end_point(getLocation());
  SectorEnd = GeoVector(Radius, EndRadial).end_point(getLocation());
}


bool 
SectorZone::isInSector(const AIRCRAFT_STATE &ref) const
{
  if (!CylinderZone::isInSector(ref)) {
    return false;
  } else {
    return angleInSector(bearing(ref.Location));
  }
}

void 
SectorZone::setStartRadial(const double x) 
{
  StartRadial = x;
  updateSector();
}

void 
SectorZone::setEndRadial(const double x) 
{
  EndRadial = x;
  updateSector();
}  

bool 
SectorZone::angleInSector(const double b) const
{
  if (StartRadial<EndRadial) {
    return ((b<=EndRadial) && (b>=StartRadial));
  } else {
    return ((b<=EndRadial) || (b>=StartRadial));
  }
}

bool
SectorZone::equals(const ObservationZonePoint* other) const
{
  if (CylinderZone::equals(other)) {
    if (const SectorZone* z = dynamic_cast<const SectorZone*>(other)) {
      return (StartRadial == z->getStartRadial()) &&
        (EndRadial == z->getEndRadial());
    }
  }
  return false;
}
