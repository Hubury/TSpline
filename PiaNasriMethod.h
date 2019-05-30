#ifndef PIANASRIMETHOD_H
#define PIANASRIMETHOD_H

#include "PiaMethod.h"
class PiaNasriMethod :public PiaMethod {
public:
	PiaNasriMethod(const vector<NURBSCurve>& _curves, int _maxIterNum = 100, double _eps = 1e-5)
		:PiaMethod(_curves, _maxIterNum, _eps) {

	}

	void init() override;		// ����NUUBSCurve��ʼ��T-preimage
	void insert() override;		// ��һ������������ĵط�����ڵ㣬�ֲ���ϸ
	void calculate() override;  // ��������

public:
	void sample_fitPoints() override;
	/*virtual void fit();
	virtual void pia();*/
};
#endif // !PIANASRIMETHOD_H

