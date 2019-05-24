#ifndef MINJAEMETHOD_H
#define MINJAEMETHOD_H

#include "Skinning.h"
class MinJaeMethod : public Skinning {
public:
	MinJaeMethod(
		const vector<NURBSCurve>& _curves, 
		int _sampleNum = 100, int _maxIterNum = 20, double _eps = 1e-5)
		:Skinning(_curves),sampleNum(_sampleNum),
		maxIterNum(_maxIterNum),eps(_eps) {

	}

	void init() override;		// ����NUUBSCurve��ʼ��T-preimage
	void insert() override;		// ��T-preimage�в����м�ڵ�
	void calculate() override;  // ��������

private:
	void inter_init();          // ��ʼ���м�������
	double inter_update();        // �����м�������

private:
	const int sampleNum;
	const int maxIterNum;
	const double eps;
	map<double, map<double, Point3d>> initial_cpts;
};


#endif // !MINJAEMETHOD_H