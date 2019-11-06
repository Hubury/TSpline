#pragma once
#ifndef BSPLINEVOLUME_H
#define BSPLINEVOLUME_H
#include "Volume.h"
#include "FitPoint.hpp"

const double PI = 3.141592653;

class BsplineVolume :public Volume{
public:
	Point3d eval(double u, double v, double w) override;

	int readVolume(string) override;
	int saveVolume(string) override;

	void get_isoparam_surface(NURBSSurface& surface, double t, char dir);

	// ��׷�ӵķ�ʽ��
	static void writeFileAdd(char *filename, std::string s);

	// ���
	static double dot(Point3d v1, Point3d v2);

	// ���
	static Point3d cross(Point3d v1, Point3d v2);

	//���������Բ׶֮��ĽǶ�
	//@ ��ֵ�ľ���ֵԽ��Խ��Ļ���ʾ����Բ׶Խ��ԽԶ�� ����ֵ�ľ���ֵԽ��ԽС�Ļ� ����section_angle-����Բ�Ľ�Խ��ԽС���Ӷ�����Բ׶Խ��Խ����
	//@ ������Ծ���Խ��ԽСԲ׶Խ��ԽԶ��
	static double degreeofTwoCone(std::vector<Point3d> &cone1, std::vector<Point3d> &cone2);
	/*
	*	�ж�Բ׶�ǲ����ཻ��
	*  @����ֵΪtrue��������ཻ ,false ������ǲ��ཻ
	*/
	static bool coneIntersect(std::vector<Point3d> &cone1, std::vector<Point3d> &cone2);

	//��������һ������Բ׶�ķ���������Բ׶�ĽǶ�
	static double degreeofTwoConeWithAnotherFunc(std::vector<Point3d> &cone1, std::vector<Point3d> &cone2);

	//�õ������������С��Բ׶
	static void getconescale(Point3d &v1, Point3d &v2, std::vector<Point3d> &cone1);

	//����˷�
	static Point3d matmul(double mat[][3], Point3d v);

	//��ά�ռ��������ε�����
	static Point3d getCircumcenter(Point3d v1, Point3d v2, Point3d v3);

	//����ͶӰ�任�������ά�ռ��ϵ�����λ��
	//��Ҫ��֤v1,v2, v3 ��Ϊ�ռ��ϵĹ��ߵĵ�
	static Point3d getCircumcenterOnThreeDim(Point3d v1, Point3d v2, Point3d v3);

	//�ж�һ�������һ��
	static bool isContainAllLine(Point3d v1, Point3d v2, std::vector<Point3d> &cone1);

	//�õ������������С��Բ׶
	static void getconescaleWithAnotherFunc(Point3d &v1, Point3d &v2, std::vector<Point3d> &cone1);


	//////////////////////////////////////////////////////////////////////////
	//�ж�Բ׶����֮��֮�䲻�ཻ
	//�ཻ����true
	//���ཻ�򷵻�false
	static bool coneIntersect3D(std::vector<Point3d> &conex, std::vector<Point3d> &coney, std::vector<Point3d> &conez);


	//ͨ��control_grid����������Բ׶
	//cone Ϊ�����������߶ι��ɵ�Բ׶�ļ���
	void constructsolidcone(int seg[], std::vector<Point3d> &conex, vector<Point3d> &coney,
		vector<Point3d> &conez);

	double FindBestratioOnSoildBysearch(const vector<vector<vector<Point3d>>>& diff_vector, vector<Point3d> &conex, vector<Point3d> &coney,
		vector<Point3d> &conez, int x_points, int y_points, int z_points);


	//���(x ,y ,z)��Ep3��ֵ
	double GetApartFuncvalueOnSoild(int x_points, int y_points, int z_points);

	Point3d getDiffofApartFuncOnSoild(int x_points, int y_points, int z_points, double delta, int x, int y, int z);

	void constructKnotVector(int x_points, int y_points, int z_points);

	//����һ�������㷵��Bi(u)Bj(v)Bk(w)
	//paraΪ�����Ĳ���
	//kont_vector�ڵ���������СΪ3����ʾxyz������
	//result��ʾ4*4*4�Ľ������ÿһά��ʾBi*Bj*Bk
	//Bi_start_index��ʾ����ֵBi�������Ļ���������ʼ�ڵ��������±�
	void calculateBaseFunctionOfSolidBSpline(Point3d para,
		vector<vector<vector<double>>>& result, int &Bi_start_index, int &Bj_start_index, int &Bk_start_index);

	// ʹ���Ż�����������һ�����ָ���㣬���ſ˱�ֵ�ܺõ�B������
	void fitBsplineSolid(vector<FitPoint3D>& fit_points, int x_points, int y_points, int z_points, double alpha, double delta);

	//����ϵ��������
	double GetSoildFiterror(vector<FitPoint3D>& fit_points,
		int x_points, int y_points, int z_points, double alpha, double delta);

	static double getConeAnglerror(std::vector<Point3d> &cones);

	static double getConeAngleandAngle(std::vector<Point3d> &conex, std::vector<Point3d> &coney);

	void lspia(vector<FitPoint3D>& fit_points, int x_points, int y_points, int z_points, int max_iter_num = 100, double eps = 1e-5);
private:
	void drawTmesh() override;
	void drawControlpolygon() override;

public:
	vector<vector<vector<Point3d>>> control_grid;
	vector<Eigen::VectorXd> knot_vector;
	vector<vector<vector<vector<double>>>> matri;
	vector<int> Bi_start_indexs;
	vector<int> Bj_start_indexs;
	vector<int> Bk_start_indexs;
};
#endif // !BSPLINEVOLUME_H

