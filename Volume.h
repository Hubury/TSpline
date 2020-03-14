#ifndef VOLUME_H
#define VOLUME_H

#include "utility.h"
using namespace std;
using namespace igl::opengl::glfw;
using namespace t_mesh;
class Volume {
public:
	Volume():id(-1), reverse(false){}
	void setViewer(Viewer* viewer) {
		this->viewer = viewer; // ��ͷ�ļ�������������cpp����inline���ᵼ�������ļ��Ҳ�������
	}
	void draw(bool tmesh, bool polygon, bool surface, double resolution = 0.01);
	// calculate the coordinate at paramter (u,v,w)
	virtual Point3d eval(double u, double v, double w) = 0;

	// read volume from file
	virtual int readVolume(string) = 0;
	// save volume to file
	virtual int saveVolume(string) = 0;
	// ͨ����������resolution�ָ������ɢΪ����������
	int saveAsHex(string, double resolution = 0.01);
	void setReverse(bool _reverse) {
		reverse = _reverse;
	}
protected:
	virtual void drawTmesh() = 0;
	virtual void drawControlpolygon() = 0;
	virtual void drawParamCurve() = 0;
	virtual void drawVolume(double resolution = 0.01);

protected:
	Viewer* viewer;
	int id;
	bool reverse;   // u,v,w�Ƿ��������ַ��򣬲���������Ҫ����Ϊtrue
};


#endif // !VOLUME_H

