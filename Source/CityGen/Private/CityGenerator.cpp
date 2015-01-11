#include "CityGenPch.h"
#include "CityGenerator.h"
#include "DrawDebugHelpers.h"
#include "ProceduralBuilding.h"

VoronoiGenerator::VoronoiGenerator() 
{

}

void VoronoiGenerator::Generate(const FVector2D MinBoundingBox, const FVector2D MaxBoundingBox)
{
	X0 = MinBoundingBox.X;
	Y0 = MinBoundingBox.Y;
	X1 = MaxBoundingBox.X;
	Y1 = MaxBoundingBox.Y;

	// Process the queues; select the top element with smaller x coordinate.
	while (!points.empty())
	{
		if (!events.empty() && events.top()->x <= points.top().x)
		{
			process_event();
		}
		else
		{
			process_point();
		}
	}

	// After all points are processed, do the remaining circle events.
	while (!events.empty())
	{
		process_event();
	}

	finish_edges(); // Clean up dangling edges.
}

void VoronoiGenerator::process_point()
{
	// Get the next point from the queue.
	point p = points.top();
	points.pop();

	// Add a new arc to the parabolic front.
	front_insert(p);
}

void VoronoiGenerator::process_event()
{
	// Get the next event from the queue.
	event *e = events.top();
	events.pop();

	if (e->valid) 
	{
		// Start a new edge.
		seg *s = new seg(e->p, e->Point, e->PrevPoint, this);

		// Remove the associated arc from the front.
		arc *a = e->a;
		if (a->prev) 
		{
			a->prev->next = a->next;
			a->prev->s1 = s;
		}
		
		if (a->next) 
		{
			a->next->prev = a->prev;
			a->next->s0 = s;
		}

		// Finish the edges before and after a.
		if (a->s0) a->s0->finish(e->p);
		if (a->s1) a->s1->finish(e->p);

		// Recheck circle events on either side of p:
		if (a->prev) check_circle_event(a->prev, e->x, e->Point, e->PrevPoint);
		if (a->next) check_circle_event(a->next, e->x, e->Point, e->PrevPoint);
	}
	delete e;
}

void VoronoiGenerator::front_insert(point p)
{
	if (!root) 
	{
		root = new arc(p);
		return;
	}

	// Find the current arc(s) at height p.y (if there are any).
	for (arc *i = root; i; i = i->next) 
	{
		point z, zz;
		if (intersect(p, i, &z)) 
		{
			// New parabola intersects arc i.  If necessary, duplicate i.
			if (i->next && !intersect(p, i->next, &zz)) 
			{
				i->next->prev = new arc(i->p, i, i->next);
				i->next = i->next->prev;
			}
			else
			{
				i->next = new arc(i->p, i);
			}

			i->next->s1 = i->s1;

			// Add p between i and i->next.
			i->next->prev = new arc(p, i, i->next);
			i->next = i->next->prev;

			i = i->next; // Now i points to the new arc.

			// Add new half-edges connected to i's endpoints.
			i->prev->s1 = i->s0 = new seg(z, p, point(), this);
			i->next->s0 = i->s1 = new seg(z, p, point(), this);

			// Check for new circle events around the new arc:
			check_circle_event(i, p.x, p, i->prev->p);
			check_circle_event(i->prev, p.x, p, i->prev->p);
			check_circle_event(i->next, p.x, p, i->prev->p);

			return;
		}
	}

	// Special case: If p never intersects an arc, append it to the list.
	arc *i;
	for (i = root; i->next; i = i->next); // Find the last node.

	i->next = new arc(p, i);
	// Insert segment between p and i
	point start;
	start.x = X0;
	start.y = (i->next->p.y + i->p.y) / 2;
	i->s1 = i->next->s0 = new seg(start, p, i->next->p, this);
}

// Look for a new circle event for arc i.
void VoronoiGenerator::check_circle_event(arc *i, double x0, point Point, point PrevPoint)
{
	// Invalidate any old event.
	if (i->e && i->e->x != x0)
	{
		i->e->valid = false;
	}
		
	i->e = NULL;

	if (!i->prev || !i->next)
	{
		return;
	}

	double x;
	point o;

	if (circle(i->prev->p, i->p, i->next->p, &x, &o) && x > x0) 
	{
		// Create new event.
		i->e = new event(x, o, i);
		i->e->Point = Point;
		i->e->PrevPoint = PrevPoint;
		events.push(i->e);
	}
}

// Find the rightmost point on the circle through a,b,c.
bool VoronoiGenerator::circle(point a, point b, point c, double *x, point *o)
{
	// Check that bc is a "right turn" from ab.
	if ((b.x - a.x)*(c.y - a.y) - (c.x - a.x)*(b.y - a.y) > 0)
		return false;

	// Algorithm from O'Rourke 2ed p. 189.
	double A = b.x - a.x, B = b.y - a.y,
		C = c.x - a.x, D = c.y - a.y,
		E = A*(a.x + b.x) + B*(a.y + b.y),
		F = C*(a.x + c.x) + D*(a.y + c.y),
		G = 2 * (A*(c.y - b.y) - B*(c.x - b.x));

	if (G == 0) return false;  // Points are co-linear.

	// Point o is the center of the circle.
	o->x = (D*E - B*F) / G;
	o->y = (A*F - C*E) / G;

	// o.x plus radius equals max x coordinate.
	*x = o->x + sqrt(pow(a.x - o->x, 2) + pow(a.y - o->y, 2));
	return true;
}

// Will a new parabola at point p intersect with arc i?
bool VoronoiGenerator::intersect(point p, arc *i, point *res)
{
	if (i->p.x == p.x) return false;

	double a, b;
	if (i->prev) // Get the intersection of i->prev, i.
		a = intersection(i->prev->p, i->p, p.x).y;
	if (i->next) // Get the intersection of i->next, i.
		b = intersection(i->p, i->next->p, p.x).y;

	if ((!i->prev || a <= p.y) && (!i->next || p.y <= b)) 
	{
		res->y = p.y;

		// Plug it back into the parabola equation.
		res->x = (i->p.x*i->p.x + (i->p.y - res->y)*(i->p.y - res->y) - p.x*p.x)
			/ (2 * i->p.x - 2 * p.x);

		return true;
	}
	return false;
}

// Where do two parabolas intersect?
point VoronoiGenerator::intersection(point p0, point p1, double l)
{
	point res, p = p0;

	if (p0.x == p1.x)
	{
		res.y = (p0.y + p1.y) / 2;
	}
	else if (p1.x == l)
	{
		res.y = p1.y;
	}
	else if (p0.x == l)
	{
		res.y = p0.y;
		p = p1;
	}
	else 
	{
		// Use the quadratic formula.
		double z0 = 2 * (p0.x - l);
		double z1 = 2 * (p1.x - l);

		double a = 1 / z0 - 1 / z1;
		double b = -2 * (p0.y / z0 - p1.y / z1);
		double c = (p0.y*p0.y + p0.x*p0.x - l*l) / z0
			- (p1.y*p1.y + p1.x*p1.x - l*l) / z1;

		res.y = (-b - sqrt(b*b - 4 * a*c)) / (2 * a);
	}
	// Plug back into one of the parabola equations.
	res.x = (p.x*p.x + (p.y - res.y)*(p.y - res.y) - l*l) / (2 * p.x - 2 * l);
	return res;
}

void VoronoiGenerator::finish_edges()
{
	// Advance the sweep line so no parabolas can cross the bounding box.
	double l = X1 + (X1 - X0) + (Y1 - Y0);

	// Extend each remaining segment to the new parabola intersections.
	for (arc *i = root; i->next; i = i->next)
	{
		if (i->s1)
		{
			i->s1->finish(intersection(i->p, i->next->p, l * 2));
		}
	}
}

void VoronoiGenerator::DrawOutput()
{
	for (vector<seg*>::iterator i = output.begin(); i != output.end(); i++)
	{
		point p0 = (*i)->start;
		point p1 = (*i)->end;
		point Point = (*i)->LeftPoint;
	
		if (Results.count(Point))
		{
			Results.at(Point).push_back(*i);
		}
		else
		{
			vector<seg*> Segments;
			Segments.push_back(*i);
			Results.insert(std::make_pair(Point, Segments));
		}

		point PrevPoint = (*i)->RightPoint;
		if (PrevPoint.x != 0 && PrevPoint.y != 0)
		{
				if (Results.count(PrevPoint))
				{
					Results.at(PrevPoint).push_back(*i);
				}
				else
				{
					vector<seg*> Segments;
					Segments.push_back(*i);
					Results.insert(std::make_pair(PrevPoint, Segments));
				}
		}
		
		DrawDebugPoint(World, FVector(Point.x, Point.y, 100), 10, FColor(0, 0, 255, 255), false, 20);
		DrawDebugLine(World, FVector(p0.x, p0.y, 100), FVector(p1.x, p1.y, 100), FColor(255, 0, 0, 255), false, 20);
	}

	for (map<point, vector<seg*>>::iterator MapItr = Results.begin(); MapItr != Results.end(); MapItr++)
	{
		vector<seg*> Segments = (*MapItr).second;
		point CorePoint = (*MapItr).first;
		TArray<FVector> Points;
		for (vector<seg*>::iterator SegItr = Segments.begin(); SegItr != Segments.end(); SegItr++)
		{
			point SegPoint = (*SegItr)->start;
			DrawDebugLine(World, FVector(CorePoint.x, CorePoint.y, 100), FVector(SegPoint.x, SegPoint.y, 100), FColor(0, 255, 0, 255), false, 20);
			SegPoint = (*SegItr)->end;
			DrawDebugLine(World, FVector(CorePoint.x, CorePoint.y, 100), FVector(SegPoint.x, SegPoint.y, 100), FColor(0, 255, 255, 255), false, 20);
			Points.Add(FVector(SegPoint.x, SegPoint.y, 0));
		}
		
		FVector CoreBuildingLocation = FVector(CorePoint.x, CorePoint.y, 0);
		TArray<FProceduralMeshTriangle> Triangles;
		AProceduralBuilding* Building = World->SpawnActor<AProceduralBuilding>();
		Building->SetActorLocation(CoreBuildingLocation);
		Building->Generate(Points, CoreBuildingLocation, Triangles);
	}
}