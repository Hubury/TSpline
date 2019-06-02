#ifndef VOLUMESKINNING_H
#define VOLUMESKINNING_H

#include "TsplineVolume.h"
#include "NURBSCurve.h"
class VolumeSkinning {
public:
	VolumeSkinning(const vector<Mesh3d>& _surfaces) :surfaces(_surfaces) {
		surfaces_num = surfaces.size();
	}
	
	virtual void parameterize();    // ���߲������õ�w������� w_params
	virtual void init();        // ����T���������ʼ��������
	virtual void insert();      // ���������в����м���
	virtual void calculate();       // ��������

	// update coordinates of control points by the formula from (nasri 2012)
	// aX + bW + cY = V
	void update();

	// ����skinning��������ʾ�м���
	void setViewer(Viewer* _viewer) {
		viewer = _viewer;
	}
private:
	Point3d centerOfmesh(const Mesh3d& mesh);
public:
	TsplineVolume volume;
protected:
	vector<Mesh3d> surfaces;
	int surfaces_num;
	Eigen::VectorXd w_params;
	Viewer* viewer;
};


#endif // !VOLUMESKINNING_H

