#ifndef VEvent_h
#define VEvent_h

#include <iostream>
#include "VPoint.h"
#include "VParabola.h"

/*
	The class for storing place / circle event in event queue.

	point		: the point at which current event occurs (top circle point for circle event, focus point for place event)
	pe			: whether it is a place event or not
	y			: y coordinate of "point", events are sorted by this "y"
	arch		: if "pe", it is an arch above which the event occurs
*/

class VEvent
{
public:
	VPoint *	point;
	bool		pe;
	double		y;
	VParabola * arch;

	/*
		Constructor for the class
		
		p		: point, at which the event occurs
		pev		: whether it is a place event or not
	*/
	
	VEvent(VPoint * p, bool pev)
	{
		point	= p;
		pe		= pev;
		y		= p->y;
		arch	= 0;
	}

	/*
		function for comparing two events (by "y" property)
	*/

	struct CompareEvent : public std::binary_function<VEvent*, VEvent*, bool>
	{
		bool operator()(const VEvent* l, const VEvent* r) const { return (l->y < r->y); }
	};
};

#endif
