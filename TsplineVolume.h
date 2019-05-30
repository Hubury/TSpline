#ifndef TSPLINEVOLUME_H
#define TSPLINEVOLUME_H
#include "utility.h"

using namespace std;
using namespace igl::opengl::glfw;
using namespace t_mesh;
class TsplineVolume {
public:
	void setViewer(Viewer* viewer) {
		this->viewer = viewer; // ��ͷ�ļ�������������cpp����inline���ᵼ�������ļ��Ҳ�������
	}
	void draw(bool tmesh, bool polygon, bool surface, double resolution = 0.01);
	Point3d eval(double u, double v, double w);

	int readVolume(string);
	int saveVolume(string);
	int saveAsHex(string, double resolution = 0.01);
private:
	void drawTmesh();
	void drawControlpolygon();
	void drawVolume(double resolution = 0.01);

private:
	map<double, Mesh3d*>       w_map;   
	Eigen::VectorXd            w_knots;     // w��ڵ�����
	Eigen::MatrixXd V;
	Eigen::MatrixXi F;
	Eigen::MatrixXi Hex;
	Viewer* viewer;
	
};

#endif // !TSPLINEVOLUME_H

