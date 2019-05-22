#ifndef PIAMETHOD_H
#define PIAMETHOD_H

#include "skinning.h"
class PiaMethod : public Skinning {
public:
	PiaMethod(const vector<NURBSCurve>& _curves)
		:Skinning(_curves) {

	}
	void init() override;		// ����NUUBSCurve��ʼ��T-preimage
	void insert() override;		// ��T-preimage�в����м�ڵ�
	void calculate() override;  // ��������
};
#endif // !PIAMETHOD_H

