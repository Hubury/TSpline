#ifndef NASRIMETHOD_H
#define NASRIMETHOD_H

#include "Skinning.h"
class NasriMethod : public Skinning {
public:

	NasriMethod(const vector<NURBSCurve>& _curves)
		:Skinning(_curves) {

	}

	void init() override;
	// 插入一列中间线
	void insert() override;
};


#endif // !NASRIMETHOD_H

