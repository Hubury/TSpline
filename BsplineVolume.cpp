#include "BsplineVolume.h"

Point3d BsplineVolume::eval(double u, double v, double w)
{
	Point3d res;
	// ֻ����� 4*4*4����
	int uid = FindSpan(knot_vector[0], u);
	int vid = FindSpan(knot_vector[1], v);
	int wid = FindSpan(knot_vector[2], w);

	for (int i = uid - 1; i <= uid + 2; i++) {
		for (int j = vid - 1; j <= vid + 2; j++) {
			for (int k = wid - 1; k <= wid + 2; k++) {
				double blend = Basis(knot_vector[0], u, i - 2)*Basis(knot_vector[1], v, j - 2)*Basis(knot_vector[2], w, k - 2);
				res.add(control_grid[i-2][j-2][k-2] * blend);
			}
		}
	}
	return res;
}

int BsplineVolume::readVolume(string filename)
{
	ifstream in(filename);
	if (!in.is_open()) {
		cout << "failed to open file: " << filename << endl;
		return -1;
	}
	
	string sline;
	getline(in, sline);
	getline(in, sline);
	istringstream sin1(sline);
	int x_points, y_points, z_points;
	sin1 >> x_points >> y_points >> z_points;
	control_grid.resize(x_points);
	for (int i = 0; i < x_points; i++)
	{
		control_grid[i].resize(y_points);
		for (int j = 0; j < y_points; j++)
		{
			control_grid[i][j].resize(z_points);
		}
	}
	knot_vector.resize(3);
	knot_vector[0].resize(x_points + 4);
	knot_vector[1].resize(y_points + 4);
	knot_vector[2].resize(z_points + 4);

	getline(in, sline);
	for (int i = 0; i < x_points; i++)
	{
		for (int j = 0; j < y_points; j++)
		{
			for (int k = 0; k < z_points; k++)
			{
				getline(in, sline);
				istringstream sin2(sline);
				sin2 >> control_grid[i][j][k][0] >> control_grid[i][j][k][1] >> control_grid[i][j][k][2];
			}
		}
	}
	getline(in, sline);
	getline(in, sline);
	istringstream sin3(sline);
	for (int i = 0; i < x_points + 4; i++)
	{
		sin3 >> knot_vector[0][i];
	}
	getline(in, sline);
	getline(in, sline);
	istringstream sin4(sline);
	for (int i = 0; i < y_points + 4; i++)
	{
		sin4 >> knot_vector[1][i];
	}
	getline(in, sline);
	getline(in, sline);
	istringstream sin5(sline);
	for (int i = 0; i < z_points + 4; i++)
	{
		sin5 >> knot_vector[2][i];
	}
	in.close();
	cout << filename + " load successful!" << endl;
	return 0;
	
}

int BsplineVolume::saveVolume(string filename)
{
	ofstream out(filename + ".vol");
	if (!out.is_open()) {
		cout << "failed to create or open file: " << filename << ".vol" << endl;
		return -1;
	}
	const int x_num = control_grid.size();
	const int y_num = control_grid[0].size();
	const int z_num = control_grid[0][0].size();

	out << "#resolution of the control grid of BsplineVolume" << endl;
	out << x_num << " " << y_num << " " << z_num << endl;
	out << "#control points of BsplineVolume" << endl;
	for (int i = 0; i < x_num; i++) {
		for (int j = 0; j < y_num; j++) {
			for (int k = 0; k < z_num; k++) {
				out << control_grid[i][j][k][0] << " " << control_grid[i][j][k][1] << " " << control_grid[i][j][k][2] << endl;
			}
		}
	}
	out << "#knot vector in u - direction of BsplineVolume" << endl;
	for (int i = 0; i < knot_vector[0].size(); i++) {
		out << knot_vector[0][i] << " ";
	}
	out << endl;
	out << "#knot vector in v - direction of BsplineVolume" << endl;
	for (int i = 0; i < knot_vector[1].size(); i++) {
		out << knot_vector[1][i] << " ";
	}
	out << endl;
	out << "#knot vector in w - direction of BsplineVolume" << endl;
	for (int i = 0; i < knot_vector[2].size(); i++) {
		out << knot_vector[2][i] << " ";
	}
	out << endl;
	out.close();
	cout << filename + " write successful!" << endl;
	return 0;
}

// ��ȡ�Ȳ���
void BsplineVolume::get_isoparam_surface(NURBSSurface & surface, double t, char dir)
{
	surface.isRational = false;
	surface.u_order = 4;
	surface.v_order = 4;
	surface.dimension = 3;
	if (dir == 'u') {
		surface.u_num = control_grid[0].size() - 1;
		surface.v_num = control_grid[0][0].size() - 1;
		surface.uknots = knot_vector[1];
		surface.vknots = knot_vector[2];
		surface.controlPw.resize(surface.v_num + 1);
		int id = FindSpan(knot_vector[0], t);
		for (int i = 0; i <= surface.v_num; i++) {
			surface.controlPw[i] = MatrixXd::Zero(surface.u_num + 1, surface.dimension);
			for (int j = 0; j <= surface.u_num; j++) {
				for (int k = id - 1; k <= id + 2; k++) {
					surface.controlPw[i].row(j) += control_grid[k - 2][j][i].toVectorXd() * Basis(knot_vector[0], t, k - 2);
				}
				
			}
		}
		
	}
	else if (dir == 'v') {
		surface.u_num = control_grid.size() - 1;
		surface.v_num = control_grid[0][0].size() - 1;
		surface.uknots = knot_vector[0];
		surface.vknots = knot_vector[2];
		surface.controlPw.resize(surface.v_num + 1);
		int id = FindSpan(knot_vector[1], t);
		for (int i = 0; i <= surface.v_num; i++) {
			surface.controlPw[i] = MatrixXd::Zero(surface.u_num + 1, surface.dimension);
			for (int j = 0; j <= surface.u_num; j++) {
				for (int k = id - 1; k <= id + 2; k++) {
					surface.controlPw[i].row(j) += control_grid[j][k - 2][i].toVectorXd() * Basis(knot_vector[1], t, k - 2);
				}

			}
		}
	}
	else if (dir == 'w') {
		surface.u_num = control_grid.size() - 1;
		surface.v_num = control_grid[0].size() - 1;
		surface.uknots = knot_vector[0];
		surface.vknots = knot_vector[1];
		surface.controlPw.resize(surface.v_num + 1);
		int id = FindSpan(knot_vector[2], t);
		for (int i = 0; i <= surface.v_num; i++) {
			surface.controlPw[i] = MatrixXd::Zero(surface.u_num + 1, surface.dimension);
			for (int j = 0; j <= surface.u_num; j++) {
				for (int k = id - 1; k <= id + 2; k++) {
					surface.controlPw[i].row(j) += control_grid[j][i][k - 2].toVectorXd() * Basis(knot_vector[2], t, k - 2);
				}

			}
		}
	}
}


void BsplineVolume::writeFileAdd(char *filename, std::string s)//��׷�ӵķ�ʽ��
{
	//char filename[]="SceneInterface.txt";
	std::ofstream fout(filename, std::ios::app);
	fout << s;
	fout.close();
}

// ���
double BsplineVolume::dot(Point3d v1, Point3d v2) {
	double res = 0;
	res += v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
	return res;
}
// ���
Point3d BsplineVolume::cross(Point3d v1, Point3d v2) {
	Point3d res;
	res[0] = v1[1] * v2[2] - v2[1] * v1[2];
	res[1] = v1[2] * v2[0] - v2[2] * v1[0];
	res[2] = v1[0] * v2[1] - v2[0] * v1[1];

	return res;
}

bool BsplineVolume::coneIntersect(std::vector<Point3d>& cone1, std::vector<Point3d>& cone2)
{
	//�жϾ���ԭ��������ǲ����ཻ
	double epbs = 1e-8;
	if (degreeofTwoCone(cone1, cone2) <= epbs) return false;  //���������ཻ
	else if (degreeofTwoConeWithAnotherFunc(cone1, cone2) <= epbs) return false;
	else return true;
}

//���������Բ׶֮��ĽǶ�
//@ ��ֵ�ľ���ֵԽ��Խ��Ļ���ʾ����Բ׶Խ��ԽԶ�� ����ֵ�ľ���ֵԽ��ԽС�Ļ� ����section_angle-����Բ�Ľ�Խ��ԽС���Ӷ�����Բ׶Խ��Խ����
//@ ������Ծ���Խ��ԽСԲ׶Խ��ԽԶ��
double BsplineVolume::degreeofTwoCone(std::vector<Point3d> &cone1, std::vector<Point3d> &cone2) {
	//�жϾ���ԭ��������ǲ����ཻ
	Point3d v1, v2;
	getconescale(v1, v2, cone1); // v1, v2ΪԲ׶1��һ��ĸ��

	Point3d vv1, vv2;
	getconescale(vv1, vv2, cone2); // vv1, vv2ΪԲ׶2��һ��ĸ��

								   //��ȡcone1��cone2��ԭ��
	Point3d cone_origin1, cone_origin2;
	v1.normalize(), v2.normalize(), vv1.normalize(), vv2.normalize();
	cone_origin1 = (v1 + v2).normalize();
	cone_origin2 = (vv1 + vv2).normalize();

	double section_angle = std::acosf(dot(cone_origin1, cone_origin2));
	//����Բ׶�İ�Բ�Ľ�
	v1.normalize(), v2.normalize();
	double cone1_angle = std::acosf(dot(v1, cone_origin1));
	double cone2_angle = std::acosf(dot(vv1, cone_origin2));

	double res = cone1_angle + cone2_angle - section_angle;  //��ֵ�����ཻ ��ֵ�����ཻ

	return res / PI*180.0;
}

//��������һ������Բ׶�ķ���������Բ׶�ĽǶ�
double BsplineVolume::degreeofTwoConeWithAnotherFunc(std::vector<Point3d> &cone1, std::vector<Point3d> &cone2) {
	Point3d v1, v2;
	getconescaleWithAnotherFunc(v1, v2, cone1);

	Point3d vv1, vv2;
	getconescaleWithAnotherFunc(vv1, vv2, cone2);

	//��ȡcone1��cone2��ԭ��
	Point3d cone_origin1, cone_origin2;
	cone_origin1 = (v1 + v2).normalize();
	cone_origin2 = (vv1 + vv2).normalize();

	double section_angle = std::acosf(dot(cone_origin1, cone_origin2));
	//����Բ׶�İ�Բ�Ľ�
	v1.normalize(), v2.normalize();
	double cone1_angle = std::acosf(dot(v1, cone_origin1));
	double cone2_angle = std::acosf(dot(vv1, cone_origin2));

	double res = cone1_angle + cone2_angle - section_angle;  //��ֵ�����ཻ ��ֵ�����뽻

	return res / PI*180.0;
}

//�õ������������С��Բ׶
void BsplineVolume::getconescale(Point3d &v1, Point3d &v2, std::vector<Point3d> &cone1) {
	for (int i = 0; i<cone1.size(); i++) cone1[i] = cone1[i].normalize();
	//Բ׶�Ĵ��� Tu, Tv, Tw
	Point3d vertial_line;
	for (int i = 0; i<cone1.size(); i++) vertial_line += cone1[i];
	vertial_line.normalize();

	//��������ĽǶ�
	double angle = 2;
	for (int i = 0; i<cone1.size(); i++) {
		if (dot(cone1[i], vertial_line)<angle) {
			angle = dot(cone1[i], vertial_line);
			v1 = cone1[i];
		}
	}

	vertial_line.normalize();
	double cosa = dot(vertial_line, v1);
	double sina = sqrt(1 - cosa*cosa);

	double l = sina;
	double ol = cosa;
	Point3d vc = v1 - vertial_line*ol;
	v2 = vertial_line*ol - vc;

	v1.normalize(), v2.normalize();
}


//����˷�
Point3d BsplineVolume::matmul(double mat[][3], Point3d v) {
	double rv[3];
	for (int i = 0; i<3; i++) {
		rv[i] = 0;
		for (int j = 0; j<3; j++) {
			rv[i] += mat[i][j] * v[j];
		}
	}
	Point3d res;
	res[0] = rv[0]; res[1] = rv[1]; res[2] = rv[2];
	return res;
}

//��ά�ռ��������ε�����
Point3d BsplineVolume::getCircumcenter(Point3d v1, Point3d v2, Point3d v3) {
	double A1 = 2 * (v2[0] - v1[0]);
	double B1 = 2 * (v2[1] - v1[1]);
	double C1 = v2[0] * v2[0] + v2[1] * v2[1] - v1[1] * v1[1] - v1[0] * v1[0];

	double A2 = 2 * (v3[0] - v2[0]);
	double B2 = 2 * (v3[1] - v2[1]);
	double C2 = v3[0] * v3[0] + v3[1] * v3[1] - v2[1] * v2[1] - v2[0] * v2[0];

	double x, y;
	x = ((C1*B2) - (C2*B1)) / ((A1*B2) - (A2*B1));
	y = ((A1*C2) - (A2*C1)) / ((A1*B2) - (A2*B1));

	Point3d o;
	o[0] = x; o[1] = y;
	return o;
}




//����ͶӰ�任�������ά�ռ��ϵ�����λ��
//��Ҫ��֤v1,v2, v3 ��Ϊ�ռ��ϵĹ��ߵĵ�
Point3d BsplineVolume::getCircumcenterOnThreeDim(Point3d v1, Point3d v2, Point3d v3) {
	//�������ά�ռ��ϵı任����
	const double epbs = 1e-9;
	Point3d ux = (v2 - v1).normalize();
	Point3d uz = cross(v2 - v1, v3 - v1).normalize();
	Point3d uy = cross(uz, ux).normalize();

	double mat[3][3];
	mat[0][0] = ux[0], mat[1][0] = ux[1], mat[2][0] = ux[2];
	mat[0][1] = uy[0], mat[1][1] = uy[1], mat[2][1] = uy[2];
	mat[0][2] = uz[0], mat[1][2] = uz[1], mat[2][2] = uz[2];


	//������������ڶ�ά�ռ��ϵ�����
	Point3d vv1;
	Point3d vv2;
	vv2[0] = (v2 - v1).norm();
	//���������ڿռ��ϵ�λ��
	double r = (v3 - v1).norm();
	double cosa = dot((v3 - v1).normalize(), (v2 - v1).normalize());
	double sina = std::sqrtf(1 - cosa*cosa);
	Point3d vv3;
	vv3[0] = r * cosa;
	vv3[1] = r * sina;

	Point3d origin = getCircumcenter(vv1, vv2, vv3);

	Point3d o = matmul(mat, origin);
	o += v1;

	return o;

}


//�ж�һ�������һ��
bool BsplineVolume::isContainAllLine(Point3d v1, Point3d v2, std::vector<Point3d> &cone1) {
	if ((v1 - v2).norm() == 0) return false;
	Point3d axis = (v1 + v2).normalize();
	double maxtt = dot(v1, axis);
	for (int p = 0; p< cone1.size(); p++) {
		double tmptt = dot(cone1[p], axis);
		if (tmptt < maxtt) {
			return false;
		}
	}
	return true;
}


//�õ������������С��Բ׶
void BsplineVolume::getconescaleWithAnotherFunc(Point3d &v1, Point3d &v2, std::vector<Point3d> &cone1) {
	for (int i = 0; i< cone1.size(); i++) cone1[i] = cone1[i].normalize();

	const double epbs = 1e-9;
	//�ȼ�������еĶ�ά��
	Point3d vv1, vv2, vv3;;
	double angle = INT_MAX;
	for (int i = 0; i< cone1.size(); i++) {
		for (int j = 0; j<cone1.size(); j++) {
			if (i == j) continue;
			Point3d axis = (cone1[i] + cone1[j]).normalize();
			double tmp = dot(axis, cone1[i]);
			if (tmp<angle)  vv1 = cone1[i], vv2 = cone1[j], angle = tmp;
		}
	}

	if (isContainAllLine(vv1, vv2, cone1)) {
		v1 = vv1, v2 = vv2;
		return;
	}

	double radius = INT_MIN;
	for (int i = 0; i< cone1.size(); i++) {
		Point3d v = cone1[i];
		if (cross(v1 - v, v2 - v).norm() <= epbs) continue;

		Point3d wo = getCircumcenterOnThreeDim(vv1, vv2, cone1[i]);
		if ((wo - vv1).norm()>radius)
			vv3 = cone1[i];
	}

	Point3d wo = getCircumcenterOnThreeDim(vv1, vv2, vv3);

	//���������Բ׶
	Point3d vertial_line = wo.normalize();
	angle = 2;
	for (int i = 0; i<cone1.size(); i++) {
		if (dot(cone1[i], vertial_line)<angle) {
			angle = dot(cone1[i], vertial_line);
			v1 = cone1[i];
		}
	}

	vertial_line.normalize();
	double cosa = dot(vertial_line, v1);
	double sina = sqrt(1 - cosa*cosa);

	double l = sina;
	double ol = cosa;
	Point3d vc = v1 - vertial_line*ol;
	v2 = vertial_line*ol - vc;

}


bool BsplineVolume::coneIntersect3D(std::vector<Point3d>& conex, std::vector<Point3d>& coney, std::vector<Point3d>& conez)
{
	if (coneIntersect(conex, coney))  return true;
	if (coneIntersect(coney, conez))  return true;
	if (coneIntersect(conez, conex))  return true;

	return false;
}

//ͨ��control_grid����������Բ׶
//cone Ϊ�����������߶ι��ɵ�Բ׶�ļ���
void BsplineVolume::constructsolidcone(int seg[], std::vector<Point3d>& conex, vector<Point3d>& coney, vector<Point3d>& conez)
{
	conex.clear(), coney.clear(), conez.clear();
	for (int i = 0; i<seg[0]; i++) {
		for (int j = 0; j <= seg[1]; j++) {
			for (int k = 0; k <= seg[2]; k++) {
				Point3d v = control_grid[i + 1][j][k] - control_grid[i][j][k];
				v.normalize();
				conex.push_back(v);
			}
		}
	}

	for (int i = 0; i <= seg[0]; i++) {
		for (int j = 0; j<seg[1]; j++) {
			for (int k = 0; k <= seg[2]; k++) {
				Point3d v = control_grid[i][j + 1][k] - control_grid[i][j][k];
				v.normalize();
				coney.push_back(v);
			}
		}
	}

	for (int i = 0; i <= seg[0]; i++) {
		for (int j = 0; j <= seg[1]; j++) {
			for (int k = 0; k< seg[2]; k++) {
				Point3d v = control_grid[i][j][k + 1] - control_grid[i][j][k];
				v.normalize();
				conez.push_back(v);
			}
		}
	}
}

double BsplineVolume::FindBestratioOnSoildBysearch(const vector<vector<vector<Point3d>>>& diff_vector, vector<Point3d>& conex, vector<Point3d>& coney, vector<Point3d>& conez, int x_points, int y_points, int z_points)
{
	double l = 0.0f, r = 1.0f;
	const double epbs = 1e-7;
	int seg[3] = { x_points - 1 ,y_points - 1 ,z_points - 1 };
	while (r - l >= epbs) {
		double mid = l + (r - l) / 2;

		for (int i = 1; i<x_points - 1; i++) {
			for (int j = 1; j< y_points - 1; j++) {
				for (int k = 1; k<z_points - 1; k++) {
					control_grid[i][j][k] = control_grid[i][j][k] - diff_vector[i][j][k]*mid;
				}
			}
		}

		constructsolidcone(seg, conex, coney, conez);
		bool f = coneIntersect3D(conex, coney, conez);

		if (!f) l = mid;
		else r = mid;

		//������ԭʼ��
		for (int i = 1; i<x_points - 1; i++) {
			for (int j = 1; j< y_points - 1; j++) {
				for (int k = 1; k<z_points - 1; k++) {
					control_grid[i][j][k] = control_grid[i][j][k] + diff_vector[i][j][k]*mid;
				}
			}
		}
	}

	return l;
}

//���(x ,y ,z)��Ep3��ֵ
double BsplineVolume::GetApartFuncvalueOnSoild(int x_points, int y_points, int z_points)
{
	Point3d xx, yy, zz;
	for (int i = 0; i<x_points - 1; i++) {
		for (int j = 0; j<y_points; j++) {
			for (int k = 0; k< z_points; k++) {
				Point3d v = control_grid[i + 1][j][k] - control_grid[i][j][k];
				v.normalize();
				xx += v;
			}
		}
	}
	xx.normalize();

	for (int i = 0; i<x_points; i++) {
		for (int j = 0; j<y_points - 1; j++) {
			for (int k = 0; k<z_points - 1; k++) {
				Point3d v = control_grid[i][j + 1][k] - control_grid[i][j][k];
				v.normalize();
				yy += v;
			}
		}
	}
	yy.normalize();


	for (int i = 0; i<x_points; i++) {
		for (int j = 0; j<y_points; j++) {
			for (int k = 0; k<z_points - 1; k++) {
				Point3d v = control_grid[i][j][k + 1] - control_grid[i][j][k];
				v.normalize();
				zz += v;
			}
		}
	}
	zz.normalize();

	return dot(xx, yy)*dot(xx, yy) + dot(xx, zz)*dot(xx, zz) + dot(yy, zz)*dot(yy, zz);
}

//�õ���ά�ϵĲ��
//control_grid Ϊ���ƶ��� 
Point3d BsplineVolume::getDiffofApartFuncOnSoild(int x_points, int y_points, int z_points, double delta, int x, int y, int z)
{
	Point3d res;
	double originv = GetApartFuncvalueOnSoild(x_points, y_points, z_points);
	for (int ax = 0; ax<3; ax++) {
		control_grid[x][y][z][ax] += delta;
		double curv = GetApartFuncvalueOnSoild(x_points, y_points, z_points);
		res[ax] = (curv - originv) / delta;
		control_grid[x][y][z][ax] -= delta;
	}

	return res;
}

void BsplineVolume::constructKnotVector(int x_points, int y_points, int z_points)
{
	knot_vector.clear();
	knot_vector = vector<VectorXd>(3);
	knot_vector[0] = VectorXd::Zero(x_points + 4);
	for (int i = 1; i <= x_points - 4; i++) {
		knot_vector[0](i + 3) = 1.0 * i / (x_points - 3);
	}
	for (int i = 0; i < 4; i++) {
		knot_vector[0](x_points + i) = 1.0;
	}

	knot_vector[1] = VectorXd::Zero(y_points + 4);
	for (int i = 1; i <= y_points - 4; i++) {
		knot_vector[1](i + 3) = 1.0 * i / (y_points - 3);
	}
	for (int i = 0; i < 4; i++) {
		knot_vector[1](y_points + i) = 1.0;
	}

	knot_vector[2] = VectorXd::Zero(z_points + 4);
	for (int i = 1; i <= z_points - 4; i++) {
		knot_vector[2](i + 3) = 1.0 * i / (z_points - 3);
	}
	for (int i = 0; i < 4; i++) {
		knot_vector[2](z_points + i) = 1.0;
	}
}


//����һ�������㷵��Bi(u)Bj(v)Bk(w)
//paraΪ�����Ĳ���
//kont_vector�ڵ���������СΪ3����ʾxyz������
//result��ʾ4*4*4�Ľ������ÿһά��ʾBi*Bj*Bk
//Bi_start_index��ʾ����ֵBi�������Ļ���������ʼ�ڵ��������±�
void BsplineVolume::calculateBaseFunctionOfSolidBSpline(Point3d para, vector<vector<vector<double>>>& result, int & Bi_start_index, int & Bj_start_index, int & Bk_start_index)
{
	assert(knot_vector.size() == 3);
	//result�����Լ���֤��4*4*4��С
	std::vector<double>Bi, Bj, Bk;
	Bi.resize(4);
	Bj.resize(4);
	Bk.resize(4);
	
	int uid = FindSpan(knot_vector[0], para[0]);
	int vid = FindSpan(knot_vector[1], para[1]);
	int wid = FindSpan(knot_vector[2], para[2]);

	Bi_start_index = uid - 3;
	Bj_start_index = vid - 3;
	Bk_start_index = wid - 3;

	for (int i = 0; i < 4; i++) {
		Bi[i] = Basis(knot_vector[0], para[0], uid - 3 + i);
	}

	for (int i = 0; i < 4; i++) {
		Bj[i] = Basis(knot_vector[1], para[1], vid - 3 + i);
	}

	for (int i = 0; i < 4; i++) {
		Bk[i] = Basis(knot_vector[2], para[2], wid - 3 + i);
	}

	//��Bi*Bj*Bk������
	for (int i = 0; i<4; i++)
		for (int j = 0; j<4; j++)
			for (int k = 0; k<4; k++)
				result[i][j][k] = Bi[i] * Bj[j] * Bk[k];
}

void BsplineVolume::fitBsplineSolid(vector<FitPoint3D>& fit_points, int x_points, int y_points, int z_points, double alpha, double delta)
{
	//��Ҫ����move_step��ֵ

	std::ostringstream os;
	vector<vector<vector<Point3d>>> move_step(x_points, vector<vector<Point3d>>(y_points, vector<Point3d>(z_points)));

	//writeFileAdd("debug.txt" ,os.str());
	constructKnotVector(x_points, y_points, z_points);
	//cout << "----1----" << endl;
	//Efit ��move_step��Ӱ��
	//double matri[4][4][4];
	//os<<para.size()<<std::endl;
	if (matri.empty()) {
		matri = vector<vector<vector<vector<double>>>>(fit_points.size());
		for (int i = 0; i < matri.size(); i++) {
			matri[i] = vector<vector<vector<double>>>(4, vector<vector<double>>(4, vector<double>(4)));
		}
		Bi_start_indexs = vector<int>(fit_points.size());
		Bj_start_indexs = vector<int>(fit_points.size());
		Bk_start_indexs = vector<int>(fit_points.size());
		for (int pn = 0; pn < fit_points.size(); pn++) {
			calculateBaseFunctionOfSolidBSpline(fit_points[pn].param, matri[pn], Bi_start_indexs[pn], Bj_start_indexs[pn], Bk_start_indexs[pn]);
		}
	}
	int Bi_start_index, Bj_start_index, Bk_start_index;
	for (int pn = 0; pn<fit_points.size(); pn++) {
		//calculateBaseFunctionOfSolidBSpline(fit_points[pn].param, matri, Bi_start_index, Bj_start_index, Bk_start_index);
		//cout << "----1-1----" << endl;
		//writeFileAdd("debug.txt",os.str());
		//Efit
		Bi_start_index = Bi_start_indexs[pn];
		Bj_start_index = Bj_start_indexs[pn];
		Bk_start_index = Bk_start_indexs[pn];

		Point3d phy_tet_pos = fit_points[pn].origin;
		double x = 0, y = 0, z = 0;
		for (int i = Bi_start_index; i<4 + Bi_start_index; i++) {
			for (int j = Bj_start_index; j< 4 + Bj_start_index; j++) {
				for (int k = Bk_start_index; k< 4 + Bk_start_index; k++) {
					x += (matri[pn][i - Bi_start_index][j - Bj_start_index][k - Bk_start_index])*control_grid[i][j][k][0];
					y += (matri[pn][i - Bi_start_index][j - Bj_start_index][k - Bk_start_index])*control_grid[i][j][k][1];
					z += (matri[pn][i - Bi_start_index][j - Bj_start_index][k - Bk_start_index])*control_grid[i][j][k][2];
				}
			}
		}
		//cout << "----1-2----" << endl;
		x -= phy_tet_pos[0], y -= phy_tet_pos[1], z -= phy_tet_pos[2];

		/*os<< x<<" "<<y<<" "<<z<<std::endl;
		writeFileAdd("debug.txt",os.str());*/

		for (int i = Bi_start_index; i<4 + Bi_start_index; i++) {
			for (int j = Bj_start_index; j<4 + Bj_start_index; j++) {
				for (int k = Bk_start_index; k<4 + Bk_start_index; k++) {
					move_step[i][j][k][0] += (1 - alpha - delta) * 2 * x*(matri[pn][i - Bi_start_index][j - Bj_start_index][k - Bk_start_index]);
					move_step[i][j][k][1] += (1 - alpha - delta) * 2 * y*(matri[pn][i - Bi_start_index][j - Bj_start_index][k - Bk_start_index]);
					move_step[i][j][k][2] += (1 - alpha - delta) * 2 * z*(matri[pn][i - Bi_start_index][j - Bj_start_index][k - Bk_start_index]);

					//os << i << " " << j << " " << k << " (" << move_step[i][j][k][0] << ", " << move_step[i][j][k][1] << ", " << move_step[i][j][k][2] << ")" << endl;
				}
			}
		}
	}
	//cout << "----2----" << endl;
	//e
	//Eu ,Ev ,Ew
	//���ÿ�������ϵĶ�i,j,kλ�õĿ��ƶ����Ӱ��

	Point3d x_axis, y_axis, z_axis;
	int nx = 0, ny = 0, nz = 0;
	for (int i = 0; i< x_points - 1; i++) {
		for (int j = 0; j< y_points; j++) {
			for (int k = 0; k< z_points; k++) {
				x_axis += control_grid[i + 1][j][k] - control_grid[i][j][k];
				nx++;
			}
		}
	}
	x_axis.normalize();

	for (int i = 0; i< x_points; i++) {
		for (int j = 0; j<y_points - 1; j++) {
			for (int k = 0; k<z_points; k++) {
				y_axis += control_grid[i][j + 1][k] - control_grid[i][j][k];
				ny++;
			}
		}
	}
	y_axis.normalize();

	for (int i = 0; i<x_points; i++) {
		for (int j = 0; j< y_points; j++) {
			for (int k = 0; k< z_points - 1; k++) {
				z_axis += control_grid[i][j][k + 1] - control_grid[i][j][k];
				nz++;
			}
		}
	}
	z_axis.normalize();

	//Eu,Ev,Ew�Ե� i,j,k��Ӱ��
	for (int i = 1; i< x_points - 1; i++) {
		for (int j = 0; j< y_points; j++) {
			for (int k = 0; k< z_points; k++) {

				double pdenominator = (control_grid[i + 1][j][k] - control_grid[i][j][k]).toVectorXd().squaredNorm();
				double A = pdenominator;
				double B = dot(x_axis, (control_grid[i + 1][j][k] - control_grid[i][j][k]));

				double rr = (1.0 / nx)*alpha;
				Point3d tmp = (x_axis*(double)std::sqrtf(A) - (control_grid[i + 1][j][k] - control_grid[i][j][k]) *B / (double)std::sqrtf(A))*rr;
				tmp /= A;
				
				move_step[i][j][k] += tmp;
				//os<<tmp[0]<<" "<<tmp[1]<<" "<<tmp[2]<<std::endl;


				pdenominator = (control_grid[i][j][k] - control_grid[i - 1][j][k]).toVectorXd().squaredNorm();
				A = pdenominator;
				B = dot(x_axis, (control_grid[i][j][k] - control_grid[i - 1][j][k]));

				rr = (1.0 / nx)*alpha;
				tmp = (x_axis*(double)std::sqrtf(A) - (control_grid[i][j][k] - control_grid[i - 1][j][k]) *B / (double)std::sqrtf(A))*rr;
				tmp /= A;
				
				move_step[i][j][k] -= tmp;
			}
		}
	}
	//cout << "----3----" << endl;
	//Ev
	for (int i = 0; i< x_points; i++) {
		for (int j = 1; j< y_points - 1; j++) {
			for (int k = 0; k< z_points; k++) {

				double pdenominator = (control_grid[i][j + 1][k] - control_grid[i][j][k]).toVectorXd().squaredNorm();
				double A = pdenominator;
				double B = dot(y_axis, (control_grid[i][j + 1][k] - control_grid[i][j][k]));

				double rr = (1.0 / ny)*alpha;

				Point3d tmp = (y_axis*(double)std::sqrtf(A) - (control_grid[i][j + 1][k] - control_grid[i][j][k])*B / (double)std::sqrtf(A))*rr;
				tmp /= A;

				//os<< A<<" "<<ny<<" "<<rr<<std::endl;
				move_step[i][j][k] += tmp;
				//os<<tmp[0]<<" "<<tmp[1]<<" "<<tmp[2]<<std::endl;

				pdenominator = (control_grid[i][j][k] - control_grid[i][j - 1][k]).toVectorXd().squaredNorm();
				A = pdenominator;
				B = dot(y_axis, (control_grid[i][j][k] - control_grid[i][j - 1][k]));

				rr = (1 / ny)*alpha;
				tmp = (y_axis*(double)std::sqrtf(A) - (control_grid[i][j][k] - control_grid[i][j - 1][k])*B / (double)std::sqrtf(A))*rr;
				tmp /= A;
				move_step[i][j][k] -= tmp;
			}
		}
	}
	//cout << "----4----" << endl;
	//Ew
	for (int i = 0; i< x_points; i++) {
		for (int j = 0; j< y_points; j++) {
			for (int k = 1; k< z_points - 1; k++) {

				double pdenominator = (control_grid[i][j][k + 1] - control_grid[i][j][k]).toVectorXd().squaredNorm();
				double A = pdenominator;
				double B = dot(z_axis, (control_grid[i][j][k + 1] - control_grid[i][j][k]));

				double rr = (1.0 / nz)*alpha;
				Point3d tmp = (z_axis*(double)std::sqrtf(A) - (control_grid[i][j][k + 1] - control_grid[i][j][k])*B / (double)std::sqrtf(A))*rr;
				tmp /= A;
				//os<<tmp[0]<<" "<<tmp[1]<<" "<<tmp[2]<<std::endl;

				move_step[i][j][k] += tmp;

				pdenominator = (control_grid[i][j][k] - control_grid[i][j][k - 1]).toVectorXd().squaredNorm();
				A = pdenominator;
				B = dot(z_axis, (control_grid[i][j][k] - control_grid[i][j][k - 1]));

				rr = (1.0 / nz)*alpha;
				tmp = (z_axis*(double)std::sqrtf(A) - (control_grid[i][j][k] - control_grid[i][j][k - 1])*B / (double)std::sqrtf(A))*rr;
				tmp /= A;
				move_step[i][j][k] -= tmp;
			}
		}
	}
	//cout << "----5----" << endl;

	//����Բ׶���ཻ����������


	double tt = 0.00001;
	//E3����
	for (int i = 1; i< x_points - 1; i++) {
		for (int j = 1; j<y_points - 1; j++) {
			for (int k = 1; k< z_points - 1; k++) {
				move_step[i][j][k] += getDiffofApartFuncOnSoild( x_points, y_points, z_points, tt,i, j, k)*delta;
			}
		}
	}

	//cout << "----6----" << endl;
	//�ƶ��Ĳ���
	//os<<"move_step: "<<std::endl;
	for (int i = 1; i< x_points - 1; i++) {
		for (int j = 1; j<y_points - 1; j++) {
			for (int k = 1; k<z_points - 1; k++) {
				move_step[i][j][k] = move_step[i][j][k];
				//os<<move_step[i][j][k][0] <<" "<<move_step[i][j][k][1] <<" "<<move_step[i][j][k][2]<<std::endl;
			}
		}
	}
	//os<<"######"<<std::endl;
	//cout << "----7----" << endl;
	std::vector<Point3d> conex, coney, conez;
	//double l = FindBestratioOnSoildBysearch(move_step, conex, coney, conez, x_points, y_points, z_points);
	// tooth --> l = 2.0
	// venus --> l = 0.001
	// head --> 1
	// moai --> 0.05
	// moai_multiVolume
	double l = 0.05;
	cout << "l : "<< l << endl;
	//os<<l<<std::endl;
	for (int i = 1; i< x_points - 1; i++) {
		for (int j = 1; j<y_points - 1; j++) {
			for (int k = 1; k<z_points - 1; k++) {
				control_grid[i][j][k] -= move_step[i][j][k]*l;
				//cout << i << ", " << j << ", " << k << endl;
				//os<<control_grid[i][j][k][0] <<" "<<control_grid[i][j][k][1] <<" "<<control_grid[i][j][k][2]<<std::endl;
			}
		}
	}
	//cout << "----8----" << endl;
	//writeFileAdd("debug.txt", os.str());
	//cout << "----9----" << endl;
}


//����ϵ��������
double BsplineVolume::GetSoildFiterror(vector<FitPoint3D>& fit_points,
	int x_points, int y_points, int z_points, double alpha, double delta) {
	constructKnotVector(x_points, y_points, z_points);
	double fiterror = 0;

	if (matri.empty()) {
		matri = vector<vector<vector<vector<double>>>>(fit_points.size());
		for (int i = 0; i < matri.size(); i++) {
			matri[i] = vector<vector<vector<double>>>(4, vector<vector<double>>(4, vector<double>(4)));
		}
		Bi_start_indexs = vector<int>(fit_points.size());
		Bj_start_indexs = vector<int>(fit_points.size());
		Bk_start_indexs = vector<int>(fit_points.size());
		for (int pn = 0; pn < fit_points.size(); pn++) {
			calculateBaseFunctionOfSolidBSpline(fit_points[pn].param, matri[pn], Bi_start_indexs[pn], Bj_start_indexs[pn], Bk_start_indexs[pn]);
		}
	}
	//double matrix[4][4][4];
	int Bi_start_index, Bj_start_index, Bk_start_index;

	for (int pn = 0; pn<fit_points.size(); pn++) {
		//calculateBaseFunctionOfSolidBSpline(fit_points[pn].param, matrix, Bi_start_index, Bj_start_index, Bk_start_index);
		Bi_start_index = Bi_start_indexs[pn];
		Bj_start_index = Bj_start_indexs[pn];
		Bk_start_index = Bk_start_indexs[pn];

		Point3d phy_tet_pos = fit_points[pn].origin, phy_B_pos;
		for (int i = Bi_start_index; i<Bi_start_index + 4; i++) {
			for (int j = Bj_start_index; j<Bj_start_index + 4; j++) {
				for (int k = Bk_start_index; k < Bk_start_index + 4; k++) {
					//cout << "---" << i << ", " << j << ", " << k << endl;
					//cout << control_grid.size() << endl;
					//cout << control_grid[0].size() << endl;
					//cout << control_grid[0][0].size() << endl;
					////cout << control_grid[i][j][k][0] << ", " << control_grid[i][j][k][1] << ", " << control_grid[i][j][k][2] << endl;
					//cout << matri[pn][i - Bi_start_index][j - Bj_start_index][k - Bk_start_index] << endl;
					phy_B_pos += control_grid[i][j][k] * matri[pn][i - Bi_start_index][j - Bj_start_index][k - Bk_start_index];
				}
					
			}
		}
		fiterror += (phy_B_pos - phy_tet_pos).norm();
	}
	std::vector<Point3d> conex, coney, conez;
	//��������������Բ׶�ĽǶ�
	int seg[]={x_points-1,y_points-1,z_points-1};
	constructsolidcone(seg,conex,coney ,conez);
	double coneangle=getConeAnglerror(conex)+getConeAnglerror(coney)+getConeAnglerror(conez);
	double coneandcone=getConeAngleandAngle(conex,coney)+getConeAngleandAngle(conex,conez)+getConeAngleandAngle(coney,conez);
	double res = 0;
	//res += (1 - alpha - delta)*fiterror/*+alpha*coneangle+delta*coneandcone*/;
	res = fiterror;
	cout << "angle error : " << alpha*coneangle + delta*coneandcone << endl;
	return res;
}

double BsplineVolume::getConeAnglerror(std::vector<Point3d> &cones) {
	Point3d vertial_line;
	//double temp=0;
	for (int i = 0; i<cones.size(); i++) vertial_line += cones[i];
	vertial_line.normalize();

	double temp = 0;
	for (int i = 0; i<cones.size(); i++) {
		temp += 1 - dot(cones[i], vertial_line);
	}
	temp /= cones.size();

	return temp;
}

double BsplineVolume::getConeAngleandAngle(std::vector<Point3d> &conex, std::vector<Point3d> &coney) {
	Point3d vertial_linex;
	for (int i = 0; i<conex.size(); i++) vertial_linex += conex[i];
	vertial_linex.normalize();

	Point3d vertial_liney;
	for (int i = 0; i<coney.size(); i++) vertial_liney += coney[i];
	vertial_liney.normalize();

	return dot(vertial_linex, vertial_liney) * dot(vertial_linex, vertial_liney);
}

void BsplineVolume::lspia(vector<FitPoint3D>& fit_points, int x_points, int y_points, int z_points, int max_iter_num, double eps)
{
	constructKnotVector(x_points, y_points, z_points);

	if (matri.empty()) {
		matri = vector<vector<vector<vector<double>>>>(fit_points.size());
		for (int i = 0; i < matri.size(); i++) {
			matri[i] = vector<vector<vector<double>>>(4, vector<vector<double>>(4, vector<double>(4)));
		}
		Bi_start_indexs = vector<int>(fit_points.size());
		Bj_start_indexs = vector<int>(fit_points.size());
		Bk_start_indexs = vector<int>(fit_points.size());
		for (int pn = 0; pn < fit_points.size(); pn++) {
			calculateBaseFunctionOfSolidBSpline(fit_points[pn].param, matri[pn], Bi_start_indexs[pn], Bj_start_indexs[pn], Bk_start_indexs[pn]);
		}
	}


	for (int pn = 0; pn < fit_points.size(); pn++) {
		int Bi_start_index = Bi_start_indexs[pn];
		int Bj_start_index = Bj_start_indexs[pn];
		int Bk_start_index = Bk_start_indexs[pn];

		Point3d val;
		for (int ii = Bi_start_index; ii < Bi_start_index + 4; ii++) {
			for (int jj = Bj_start_index; jj < Bj_start_index + 4; jj++) {
				for (int kk = Bk_start_index; kk < Bk_start_index + 4; kk++)
					val += control_grid[ii][jj][kk] * matri[pn][ii - Bi_start_index][jj - Bj_start_index][kk - Bk_start_index];
			}
		}
		fit_points[pn].eval = val;
	}

	for (int iter = 0; iter < max_iter_num; iter++) {

		for (int i = 0; i < x_points; i++) {
			for (int j = 0; j < y_points; j++) {
				for (int k = 0; k < z_points; k++) {
					double sum1 = 0;
					Point3d sum2;

					for (int pn = 0; pn < fit_points.size(); pn++) {
						int Bi_start_index = Bi_start_indexs[pn];
						int Bj_start_index = Bj_start_indexs[pn];
						int Bk_start_index = Bk_start_indexs[pn];

						double blend = 0.0;
						if (i >= Bi_start_index && i < Bi_start_index + 4 &&
							j >= Bj_start_index && j < Bj_start_index + 4 &&
							k >= Bk_start_index && k < Bk_start_index + 4) {
							blend = matri[pn][i - Bi_start_index][j - Bj_start_index][k - Bk_start_index];
						}

						sum1 += blend;
						Point3d delta = fit_points[pn].origin - fit_points[pn].eval; // ��ϵ㴦�������
						sum2 += blend * delta;
					}
					double factor = 0.0;
					if (abs(sum1) > 0.0001) {
						factor = 1.0 / sum1; // sum1Ϊ0ʱ����Ӧ�㲻����
					}
					sum2 *= factor; // ���Ƶ���²�����
					control_grid[i][j][k] += sum2;
				}
			}
		}
			

			double error = 0.0;
			double maxerror = -1;
			// ���Ƶ���º󣬼����µ����λ�ú����
			for (int pn = 0; pn < fit_points.size(); pn++) {
				int Bi_start_index = Bi_start_indexs[pn];
				int Bj_start_index = Bj_start_indexs[pn];
				int Bk_start_index = Bk_start_indexs[pn];

				Point3d val;
				for (int ii = Bi_start_index; ii < Bi_start_index + 4; ii++) {
					for (int jj = Bj_start_index; jj < Bj_start_index + 4; jj++) {
						for (int kk = Bk_start_index; kk < Bk_start_index + 4; kk++)
							val += control_grid[ii][jj][kk] * matri[pn][ii - Bi_start_index][jj - Bj_start_index][kk - Bk_start_index];
					}
				}
				fit_points[pn].eval = val;
				fit_points[pn].error = fit_points[pn].geterror();
				maxerror = max(maxerror, fit_points[pn].error);
				error += fit_points[pn].error;
			}
			error /= fit_points.size();
			cout << "iter: " << iter + 1 << ", error: " << error <<", maxerror: " << maxerror << endl;
			if (error < eps) {
				break;
			}
		}
}


void BsplineVolume::drawTmesh()
{
	assert(viewer != NULL); // use setViewer(Viewer* viewer)

	(*viewer).data().add_label(Eigen::Vector3d(-0.02, -0.02, -0.02), "O");
	(*viewer).data().add_label(Eigen::Vector3d(1.05, 0, 0), "u");
	(*viewer).data().add_label(Eigen::Vector3d(0, 1.05, 0), "v");
	(*viewer).data().add_label(Eigen::Vector3d(0, 0, 1.05), "w");

	

	const int x_num = control_grid.size();
	const int y_num = control_grid[0].size();
	const int z_num = control_grid[0][0].size();

	Eigen::MatrixXd points(x_num*y_num*z_num, 3);
	int count = 0;
	for (int i = 0; i < x_num; i++) {
		for (int j = 0; j < y_num; j++) {
			for (int k = 0; k < z_num; k++){
				
				Eigen::MatrixXd P1(1, 3);
				Eigen::MatrixXd P2(1, 3);
				P1 << knot_vector[0](i + 2), knot_vector[1](j + 2), knot_vector[2](k + 2);
				
				(*viewer).data().add_points(P1, red);
				points.row(count++) = P1;

				if (i != x_num - 1) {
					P2 << knot_vector[0](i + 3), knot_vector[1](j + 2), knot_vector[2](k + 2);
					(*viewer).data().add_edges(P1, P2, white);
				}
				if (j != y_num - 1) {
					knot_vector[0](i + 2), knot_vector[1](j + 3), knot_vector[2](k + 2);
					(*viewer).data().add_edges(P1, P2, white);
				}
				if (k != z_num - 1) {
					knot_vector[0](i + 2), knot_vector[1](j + 2), knot_vector[2](k + 3);
					(*viewer).data().add_edges(P1, P2, white);
				}	
			}
		}
	}
	
	
	(*viewer).core.align_camera_center(points); // center
}

void BsplineVolume::drawControlpolygon()
{
	assert(viewer != NULL); // use setViewer(Viewer* viewer)

	const int x_num = control_grid.size();
	const int y_num = control_grid[0].size();
	const int z_num = control_grid[0][0].size();

	Eigen::MatrixXd points(x_num*y_num*z_num, 3);
	
	int count = 0;
	for (int i = 0; i < x_num; i++) {
		for (int j = 0; j < y_num; j++) {
			for (int k = 0; k < z_num; k++) {
				//cout << i << ", " << j << ", " << k << endl;
				Eigen::MatrixXd P1;
				Eigen::MatrixXd P2;

				array2matrixd(control_grid[i][j][k], P1);
				(*viewer).data().add_points(P1, red);
				points.row(count++) = P1;

				if (i != x_num - 1) {
					array2matrixd(control_grid[i + 1][j][k], P2);
					(*viewer).data().add_edges(P1, P2, white);
				}
				if (j != y_num - 1) {
					array2matrixd(control_grid[i][j + 1][k], P2);
					(*viewer).data().add_edges(P1, P2, white);
				}
				if (k != z_num - 1) {
					array2matrixd(control_grid[i][j][k + 1], P2);
					(*viewer).data().add_edges(P1, P2, white);
				}
			}
		}
	}



	(*viewer).core.align_camera_center(points);

}

void BsplineVolume::drawParamCurve()
{
	cout << "to be continued..." << endl;
}
