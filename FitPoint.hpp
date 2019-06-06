#pragma once
#include "utility.h"

template<int num>
class FitPoint
{
public:
	double geterror() {
		return (origin - eval).toVectorXd().norm();
	}
	bool inRectangle(const t_mesh::Array<double, num>& low, const t_mesh::Array<double, num>& high) {
		bool res = true;
		for (int i = 0; i < num; i++) {
			if (param[i] < low[i] || param[i] > high[i]) {
				res = false;
			}
		}
		return res;
	}

public:
	Point3d origin; // Ҫ��ϵ������
	t_mesh::Array<double, num> param; // ��Ӧ����
	Point3d eval;   // �����϶�Ӧ������ֵ T(u,v) 
	double error;

};

typedef FitPoint<2> FitPoint2D;
typedef FitPoint<3> FitPoint3D;
