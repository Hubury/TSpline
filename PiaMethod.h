#ifndef PIAMETHOD_H
#define PIAMETHOD_H

#include "skinning.h"

struct FitPoint {
	Point3d origin; // Ҫ��ϵ������
	double u;       // ��Ӧ����
	double v;
	Point3d eval;   // �����϶�Ӧ������ֵ T(u,v) 
	double error; 
	double geterror() {
		return (origin - eval).toVectorXd().norm();
	}
};

class PiaMethod : public Skinning {
public:
	PiaMethod(const vector<NURBSCurve>& _curves, int _maxIterNum = 100, double _eps = 1e-5)
		:Skinning(_curves), maxIterNum(_maxIterNum), eps(_eps){

	}
	void init() override;		// ����NUUBSCurve��ʼ��T-preimage
	void insert() override;		// ��һ������������ĵط�����ڵ㣬�ֲ���ϸ
	void calculate() override;  // ��������

private:
	void sample_fitPoints();
	void fit();
	void pia();

private:
	const int maxIterNum;
	const double eps;
	double error;
	vector<FitPoint> fitPoints;
};
#endif // !PIAMETHOD_H

