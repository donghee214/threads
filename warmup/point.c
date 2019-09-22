#include <assert.h>
#include "common.h"
#include <math.h>
#include "point.h"

void
point_translate(struct point *p, double x, double y)
{
        p->x = x;
        p->y = y;
}

double
point_distance(const struct point *p1, const struct point *p2)
{       
        double xVal = p1->x - p2->x;
        double yVal = p1->y - p2->y;
        return sqrt(xVal*xVal + yVal*yVal);
}

int
point_compare(const struct point *p1, const struct point *p2)
{
    double p1Distance = sqrt(p1->x*p1->x + p1->y*p1->y);
    double p2Distance = sqrt(p2->x*p2->x + p2->y*p2->y);
    if(p1Distance < p2Distance){
        return -1;
    }
    else if(p1Distance > p2Distance){
        return 1;
    }
    else{
        return 0;
    }
	
}
