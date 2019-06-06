#ifndef PIAMETHOD_H
#define PIAMETHOD_H

#include "skinning.h"
#include "FitPoint.hpp"
//struct FitPoint {
//	Point3d origin; // Ҫ��ϵ������
//	double u;       // ��Ӧ����
//	double v;
//	Point3d eval;   // �����϶�Ӧ������ֵ T(u,v) 
//	double error; 
//	double geterror() {
//		return (origin - eval).toVectorXd().norm();
//	}
//	bool inRectangle(const std::tuple<double, double, double, double>& rect) {
//		double umin = std::get<0>(rect);
//		double umax = std::get<1>(rect);
//		double vmin = std::get<2>(rect);
//		double vmax = std::get<3>(rect);
//		return u >= umin && u <= umax && v >= vmin && v <= vmax;
//	}
//};

class PiaMethod : public Skinning {
public:
	PiaMethod(const vector<NURBSCurve>& _curves, int _maxIterNum = 100, double _eps = 1e-5)
		:Skinning(_curves), maxIterNum(_maxIterNum), eps(_eps){

	}
	void init() override;		// ����NUUBSCurve��ʼ��T-preimage
	void insert() override;		// ��һ������������ĵط�����ڵ㣬�ֲ���ϸ
	void calculate() override;  // ��������

public:
	virtual void sample_fitPoints();
	virtual void fit();
	virtual void pia();

protected:
	const int maxIterNum;
	const double eps;
	double error;
	vector<FitPoint2D> fitPoints;
};
#endif // !PIAMETHOD_H

