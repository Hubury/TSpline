#ifndef VOLUMEPIAMETHOD_H
#define VOLUMEPIAMETHOD_H

#include "VolumeSkinning.h"
#include "FitPoint.hpp"
#include "BsplineVolume.h"
class VolumePiaMethod :public VolumeSkinning {
public:
	VolumePiaMethod(const vector<Mesh3d>& _surfaces,int _maxIterNum=100,double _eps = 1e-5)
		:VolumeSkinning(_surfaces),maxIterNum(_maxIterNum),eps(_eps) {

	}

	void calculate() override;  // ��������


public:
	// ���ø�����
	void set_helper_points(const MatrixXd& points) {
		helper_points.resize(points.rows());
		for (int i = 0; i < points.rows(); i++) {
			helper_points[i].origin.fromVectorXd(points.row(i));
		}
		(*viewer).data().add_points(points, red);
	}
	// �������������
	void param_helper_points(Point3d& low, Point3d& high);
	void sample_fitPoints_2();
	void sample_fitPoints_bvolume();
	void sample_fitPoints();
	void fit();
	void pia();
	void cal_basis_cache();

private:
	const int maxIterNum;
	const double eps;
	double error;
	vector<FitPoint3D> fitPoints;
	vector<FitPoint3D> surface_points;
	vector<FitPoint3D> inter_points;
	vector<FitPoint3D> helper_points;
	vector<vector<double>> basis_cache; // (B_i(t_j)
	vector<double> basis_cache_sum; // sum_j (B_i(t_j)
};
#endif // !VOLUMEPIAMETHOD_H

