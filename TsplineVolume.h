#ifndef TSPLINEVOLUME_H
#define TSPLINEVOLUME_H
#include "Volume.h"
class TsplineVolume : public Volume{
private:
	TsplineVolume& operator=(const TsplineVolume&){}
public:
	TsplineVolume(){}
	TsplineVolume(const TsplineVolume& other);
	~TsplineVolume();
	// ��ȡ����ֵΪ(u,v,w)�ĵ������
	Point3d eval(double u, double v, double w) override;
	// read volume from file
	int readVolume(string) override;
	// save volume to file
	int saveVolume(string) override;

	// insert a layer to the T-mesh of this volume
	void insert(double w, Mesh3d* mesh);
	// get the number of nodes
	int get_num()const;
	
private:
	void drawTmesh() override;
	void drawControlpolygon() override;
	void drawParamCurve() override;
	

public:
	map<double, Mesh3d*>       w_map;       // T�������ɶ��T������ɣ�ÿ��T�����Ӧһ��w����ڵ�ֵ
	Eigen::VectorXd            w_knots;     // w��ڵ�����
	
};

#endif // !TSPLINEVOLUME_H

