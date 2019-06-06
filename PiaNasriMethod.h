#ifndef PIANASRIMETHOD_H
#define PIANASRIMETHOD_H

#include "PiaMethod.h"
#include "NURBSSurface.h"
#include <igl/point_mesh_squared_distance.h>
class PiaNasriMethod :public PiaMethod {
public:
	PiaNasriMethod(const vector<NURBSCurve>& _curves, int _maxIterNum = 100, double _eps = 1e-5)
		:PiaMethod(_curves, _maxIterNum, _eps) {

	}
	// ���ø�����
	void set_helper_points(const MatrixXd& points) {
		helper_points.resize(points.rows());
		for (int i = 0; i < points.rows(); i++) {
			helper_points[i].origin.fromVectorXd(points.row(i));
		}
	}
	// �������������
	std::tuple<double, double, double, double> param_helper_points();

	void init() override;		// ����NUUBSCurve��ʼ��T-preimage
	void insert() override;		// ��һ������������ĵط�����ڵ㣬�ֲ���ϸ
	void calculate() override;  // ��������

public:
	void sample_fitPoints_1();
	void sample_fitPoints_2();
	void sample_fitPoints() override;
	void pia() override;

private:
	vector<FitPoint2D> curve_points;
	vector<FitPoint2D> inter_points;
	vector<FitPoint2D> helper_points;
};
#endif // !PIANASRIMETHOD_H

