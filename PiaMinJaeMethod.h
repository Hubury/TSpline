#ifndef PIAMINJAEMETHOD_H
#define PIAMINJAEMETHOD_H

#include "PiaMethod.h"
class PiaMinJaeMethod : public PiaMethod {
public:
	PiaMinJaeMethod(const vector<NURBSCurve>& _curves, int _maxIterNum = 100, double _eps = 1e-5)
		:PiaMethod(_curves, _maxIterNum, _eps) {

	}
	void init() override;		// ����NUUBSCurve��ʼ��T-preimage
	void insert() override;		// ��һ������������ĵط�����ڵ㣬�ֲ���ϸ
	void calculate() override;  // ��������

public:
	void sample_fitPoints() override;
};

#endif // !PIAMINJAEMETHOD_H

