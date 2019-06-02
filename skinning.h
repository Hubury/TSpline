#ifndef SKINNING_H
#define SKINNING_H

#include "utility.h"
#include "NURBSCurve.h"
using namespace std;
using namespace t_mesh;
class Skinning {
public:
	Skinning(const vector<NURBSCurve>& _curves) :curves(_curves) {
		curves_num = curves.size();
	}
	virtual void parameterize();    // ���߲������õ�u������� u_knots
	virtual void init() = 0;        // ����NUUBSCurve��ʼ��T-preimage
	virtual void insert() = 0;      // ��T-preimage�в����м�ڵ�
	virtual void calculate();       // ��������

	// update�ĸ������������ڼ�����������������ϵ��
	void basis_split(const map<double, Node<Point3d>*>& fewer_map, const map<double, Node<Point3d>*>& more_map, map<double, Point3d>& coeff);
	
	// update coordinates of control points by the formula from (nasri 2012)
	// aX' + bW + cY' = V
	void update();

	// ����skinning��������ʾ�м���
	void setViewer(Viewer* _viewer) {
		viewer = _viewer;
	}

public:
	Mesh3d tspline;
protected:
	int curves_num;
	VectorXd s_knots;
	vector<NURBSCurve> curves;
	Viewer* viewer;

};
#endif // !SKINNING_H

