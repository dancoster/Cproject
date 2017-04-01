#include "SPPoint.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>


struct sp_point_t {
	double* data;
	int dim;
	int index;
};

SPPoint* spPointCreate(double* data, int dim, int index) {
	if (data==NULL || dim<=0 || index<0)
		return NULL;

	SPPoint *res = (SPPoint*) malloc(sizeof(SPPoint));
	if (res == NULL)											//memory allocation failure
		return NULL;

	double *dataCopy = (double*) malloc(dim*sizeof(double));
	if (dataCopy == NULL) {										//memory allocation failure
		free(res);
		return NULL;
	}

	memcpy(dataCopy, data, dim*sizeof(double));					//memory copy data to data_copy
	res->data = dataCopy;
	res->dim = dim;
	res->index = index;

	return res;

}

SPPoint* spPointCopy(SPPoint* source) {
	assert(source!=NULL);

	SPPoint *copy = spPointCreate(source->data, source->dim, source->index);
	return copy;
}

void spPointDestroy(SPPoint* point) {
	if (point == NULL)
		return;

	free(point->data);
	free(point);
	return;
}

int spPointGetDimension(SPPoint* point) {
	assert(point!=NULL);

	return point->dim;
}

int spPointGetIndex(SPPoint* point) {
	assert(point!=NULL);

	return point->index;
}

double spPointGetAxisCoor(SPPoint* point, int axis) {
	assert(point!=NULL && axis < point->dim);

	return point->data[axis];
}

double spPointL2SquaredDistance(SPPoint* p, SPPoint* q) {
	assert (p!=NULL && q!=NULL && p->dim==q->dim);

	double res = 0;
	int n = spPointGetDimension(p);
	int i;
	for (i=0; i<n; i++) {
		double p_i=spPointGetAxisCoor(p, i);
		double q_i=spPointGetAxisCoor(q, i);
		res += (p_i-q_i)*(p_i-q_i);
	}
	return res;
}
