/* Generated by Together */

#include "AATPoint.hpp"
#include "Util.h"
#include "Math/Earth.hpp"
#include "Math/Geometry.hpp"
#include "AATIsolineSegment.hpp"
#include "AATIsolineIntercept.hpp"
#include <math.h>

GEOPOINT AATPoint::get_reference_scored() const
{
  if (getActiveState() == BEFORE_ACTIVE) {
    return getMaxLocation();
  } else {
    return getMinLocation();
  }
}

GEOPOINT AATPoint::get_reference_travelled() const
{
  if (has_entered()) {
    return getMaxLocation();
  } else {
    return getMinLocation();
  }
}

GEOPOINT AATPoint::get_reference_remaining() const
{
  if (getActiveState() == BEFORE_ACTIVE) {
    return getMaxLocation();
  } else {
    return TargetLocation;
  }
}

double 
AATPoint::getElevation() const
{
  // TODO: look up elevation of target and use that instead
  return Elevation; // + SAFETYTERRAIN
}

bool 
AATPoint::update_sample(const AIRCRAFT_STATE& state) 
{
  bool retval = OrderedTaskPoint::update_sample(state);
  if (active_state == CURRENT_ACTIVE) {
    retval |= check_target(state);
  }

  return retval;
}

// target handling

bool
AATPoint::check_target(const AIRCRAFT_STATE& state) 
{
  bool moved = false;
  if (isInSector(state)) {
    moved = check_target_inside(state);
  } else {
    moved = check_target_outside(state);
  }

  return moved;
}

bool
AATPoint::check_target_inside(const AIRCRAFT_STATE& state) 
{
  // target must be moved if d(p_last,t)+d(t,p_next) 
  //    < d(p_last,state)+d(state,p_next)

  if (double_leg_distance(TargetLocation) 
      < double_leg_distance(state.Location)) 
  {
    const double d_in_front = 0.01;
    const double d_to_max = ::Distance(state.Location, 
                                       getMaxLocation());

    if (d_to_max<d_in_front) {
      // no improvement available
      return false;
    } else {
      const double p = std::max(0.0,std::min(1.0,d_in_front/d_to_max));
      TargetLocation = state.Location.interpolate(getMaxLocation(), p); 
      return true;
    }
  } else {
    return false;
  }
}

bool
AATPoint::check_target_outside(const AIRCRAFT_STATE& state) 
{
  // this is optional, to be replaced!
  return false;

  if (!get_previous()->isInSector(state)) {
    double b0s = ::Bearing(get_previous()->get_reference_remaining(),
                           state.Location);
    double bst = ::Bearing(state.Location,
                           TargetLocation);
    double dst = ::Distance(state.Location,
                            TargetLocation);
    double da = ::AngleLimit180(b0s-bst);
    if ((fabs(da)>10.0) && (dst>1.0)) {

      AATIsolineIntercept ai(*this);
      AIRCRAFT_STATE si;
      if (ai.intercept(*this, state, 0.0, si.Location)
          && isInSector(si)) {

        // Note that this fights with auto-target

        TargetLocation = si.Location;

        bst = ::Bearing(state.Location,
                        TargetLocation);
        dst = ::Distance(state.Location,
                         TargetLocation);
        return true;
      }
    }
  }
  return false;
}


void 
AATPoint::set_range(const double p)
{
  if (getActiveState() != BEFORE_ACTIVE) {
    TargetLocation = getMinLocation().interpolate(getMaxLocation(),p);
  }
}


void 
AATPoint::update_projection()
{
  OrderedTaskPoint::update_projection();  

// TODO: update ellipse if we have one saved
}


void 
AATPoint::set_target(const GEOPOINT &loc)
{
  TargetLocation = loc;
}


