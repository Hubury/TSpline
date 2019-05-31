#ifndef VOLUME_H
#define VOLUME_H

#include "utility.h"

using namespace std;
using namespace igl::opengl::glfw;
using namespace t_mesh;
class Volume {
public:
	void setViewer(Viewer* viewer) {
		this->viewer = viewer; // ��ͷ�ļ�������������cpp����inline���ᵼ�������ļ��Ҳ�������
	}
	void draw(bool tmesh, bool polygon, bool surface, double resolution = 0.01);
	virtual Point3d eval(double u, double v, double w) = 0;

	virtual int readVolume(string) = 0;
	virtual int saveVolume(string) = 0;
	int saveAsHex(string, double resolution = 0.01);
protected:
	virtual void drawTmesh() = 0;
	virtual void drawControlpolygon() = 0;
	virtual void drawVolume(double resolution = 0.01);

protected:
	Viewer* viewer;

};


#endif // !VOLUME_H

