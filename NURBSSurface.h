// This file is part of NURBS, a simple NURBS library.
// github repo: https://github.com/aijm/NURBS
// Copyright (C) 2018 Jiaming Ai <aichangeworld@gmail.com>

#ifndef NURBSSURFACE_H
#define NURBSSURFACE_H

#include <igl/opengl/glfw/Viewer.h>
#include <igl/cotmatrix.h>
#include <igl/massmatrix.h>
#include <igl/invert_diag.h>
#include <igl/gaussian_curvature.h>
#include <igl/jet.h>
#include <igl/parula.h>
#include <igl/principal_curvature.h>
#include "NURBSCurve.h"
using namespace Eigen;
using namespace std;
struct NURBSSurface
{
	NURBSSurface():id(-1),isRational(false){}

	/*input format:
	_order       : // _order(0):u direction; _order(1): v direction
	_controlP: _controlP[i] represents u direction control point ,matrix (m+1) by 3 or 4
				  v
				  |
	_controlP[n]: | P_0n P_1n ... P_mn
	_controlP[i]: | ...
	_controlP[1]: | P_01 P_11 ... P_m1
	_controlP[0]: | P_00 P_10 ... P_m0
				   ------------------------> u

	_uknots   : u_0,u_1,...,u_(m+u_order)
	_vknots   : v_0,v_1,...,v_(n+v_order)
	_isRational:          */

	NURBSSurface(VectorXi _order, vector<MatrixXd> _controlP, VectorXd _uknots, VectorXd _vknots, bool _isRational = false);

	// load
	bool loadNURBS(string);

	// save 
	bool saveNURBS(string);

	bool saveAsObj(string, double resolution = 0.01);

	// find the knot interval of t by binary searching
	int find_ind(double t, int k, int n, const VectorXd& knots);

	// calculate coordinate of curve point with parameter u & v
	MatrixXd eval(double u, double v) const;

	MatrixXd eval(double t, const MatrixXd &_controlP, const VectorXd &knots);

	// 获取等参线
	void get_isoparam_curve(NURBSCurve& curve, double t, char dir = 'u');

	// knot insertion
	bool insert(double s, char dir='u');
	// kont insertion
	bool insert(double s, double t);

	// display by libigl
	void draw(igl::opengl::glfw::Viewer& viewer, bool showpolygon=true,bool showsurface=true,double resolution = 0.01);

	// draw controlpolygon
	void drawControlPolygon(igl::opengl::glfw::Viewer &viewer);

	// draw NURBS surface
	void drawSurface(igl::opengl::glfw::Viewer &viewer, double resolution = 0.01);

	// surface skinning
	void skinning(const vector<NURBSCurve> &curves, igl::opengl::glfw::Viewer &viewer);

	// surface skinning
	void skinning(const vector<NURBSCurve> &curves,const VectorXd& curves_param, igl::opengl::glfw::Viewer &viewer);

public:
	double mean_curvature(double u, double v) const;
	double guassian_curvature(double u, double v) const;

	void curvature(double u, double v, double& k1, double& k2) const;
	void derivative(double u, double v, RowVector3d& du, RowVector3d& dv, RowVector3d& d2u, RowVector3d& d2v, RowVector3d& duv) const;

	static int FindSpan(const Eigen::MatrixXd &knots, double t, int p = 3);

public:
	bool isRational = false;
	int u_order; // order of u direction
	int v_order; // order of v direction
	int u_num; // the final index of u direction control point
	int v_num; // the final index of v direction control point
	VectorXd uknots; // knots vector u direction : u_0, u_1, ..., u_(m + u_order)
	VectorXd vknots; // knots vector v direction : v_0,v_1,...,v_(n+v_order)
	vector<MatrixXd> controlP;
	vector<MatrixXd> controlPw;
	int dimension; // the dimension of control point 2 or 3 or 4

	// use libigl mesh(V,F) structure to show the surface
	MatrixXd mesh_V;
	MatrixXi mesh_F;
	int id;
};


#endif // !NURBSSURFACE_H




