#include "Test.h"

clock_t Test::begin;
clock_t Test::end;

void Test::test_Nurbs_curvature()
{
	NURBSSurface surface;
	surface.loadNURBS("../out/nurbs/venus_front.cpt");
	surface.draw(Window::viewer, false, true, 0.01);
	Window w;
	w.launch();
}

void Test::test_TsplineSimplify()
{
	vector<int> a{ 8,9,7,4,3,5 };
	vector<int> index(a.size());
	iota(index.begin(), index.end(), 0);
	sort(index.begin(), index.end(), [a](int id1, int id2) {return a[id1] > a[id2]; });
	for (int i = 0; i < index.size(); i++) {
		cout << "id: " << index[i] << "; value: " << a[index[i]] << endl;
	}
	NURBSSurface surface;
	surface.loadNURBS("../out/nurbs/venus_front.cpt");
	//surface.draw(Window::viewer, false, true);
	/*Window w;
	w.launch();*/
	Mesh3d tspline;   
	t_mesh::TsplineSimplify(surface, tspline,20, 5e-3);
	tspline.saveMesh("../out/tspline/fuck");
	MeshRender render(&tspline, false, true, true);
	render.launch();
}

void Test::test_venus_skinning()
{
	BsplineVolume volume;
	volume.readVolume("../out/volume/venus_bspline.txt");
	const int curves_num = volume.knot_vector[2].size() - 6;
	vector<NURBSCurve> curves(curves_num);

	uniform_int_distribution<unsigned> u(15, 30);
	default_random_engine e;
	default_random_engine e1;
	uniform_real_distribution<double> u1(0, 1);

	for (int i = 0; i < curves_num; i++) {
		int sampleNum = u(e);
		cout << i << " sample " << sampleNum << " points" << endl;
		Eigen::MatrixXd points(sampleNum+2, 3);
		double z = volume.knot_vector[2](i + 3);

		vector<double> x_coors;
		x_coors.push_back(0);
		x_coors.push_back(1);
		
		for (int j = 0; j < sampleNum; j++) {
			x_coors.push_back(u1(e1));
		}
		sort(x_coors.begin(), x_coors.end());
		for (int j = 0; j < x_coors.size(); j++) {
			points.row(j) = volume.eval(x_coors[j], 0, z).toVectorXd();
		}
		curves[i].interpolate(points);
		curves[i].saveNURBS("../out/nurbs/venus_curve_" + to_string(i));
		curves[i].draw(Window::viewer, false, true, 0.001);
	}


	//Skinning* method = new PiaMethod(curves, 1000);
	//Skinning* method = new NasriMethod(curves);
	//Skinning* method = new OptMethod(curves);
	//Skinning* method = new PiaMinJaeMethod(curves, 1000);
	//Skinning* method = new PiaNasriMethod(curves, 1000);
	Skinning* method = new MinJaeMethod(curves, 40, 20);

	method->setViewer(&Window::viewer);
	method->calculate();
	Mesh3d* mesh = &(method->tspline);
	cout << "num of nodes: " << mesh->get_num() << endl;

	mesh->saveMesh("../out/tspline/venus_skinning");
	MeshRender render(mesh, false, false, true, 0.001);
	render.launch();

}

void Test::test_venus_skinning_helper_points()
{
	/*BsplineVolume volume;
	volume.readVolume("../out/volume/venus_bspline.txt");*/

	NURBSSurface surface;

	
	surface.loadNURBS("../out/nurbs/venus_front.cpt");
	surface.saveAsObj("../out/OBJ/venus_surface", 0.01);
	//surface.draw(Window::viewer, false, true, 0.01);
	////surface.saveNURBS("../out/nurbs/venus_front");
	//Window w;
	//w.launch();

	const double resolution = 0.1;
	const int curves_num = 1.0 / resolution + 1;
	vector<NURBSCurve> curves(curves_num);

	uniform_int_distribution<unsigned> u(15, 30);
	default_random_engine e;
	default_random_engine e1;
	uniform_real_distribution<double> u1(0, 1);

	for (int i = 0; i < curves_num; i++) {
		int sampleNum = u(e);
		cout << i << " sample " << sampleNum << " points" << endl;
		Eigen::MatrixXd points(sampleNum + 2, 3);
		double z = i*resolution;

		vector<double> x_coors;
		x_coors.push_back(0);
		x_coors.push_back(1);

		for (int j = 0; j < sampleNum; j++) {
			double x = 1.0*(j+1) / (sampleNum+1);
			//double x = u1(e1);
			//cout << "x: " << x << endl;
			x_coors.push_back(x);
		}
		sort(x_coors.begin(), x_coors.end());
		for (int j = 0; j < x_coors.size(); j++) {
			points.row(j) = surface.eval(x_coors[j], z);
		}
		//Window::viewer.data().add_points(points, blue);
		curves[i].interpolate(points);
		curves[i].saveNURBS("../out/nurbs/venus_curve_1_" + to_string(i));
		curves[i].draw(Window::viewer, false, true,0.001);
	}

	/*NURBSSurface skin;
	skin.skinning(curves, Window::viewer);
	skin.draw(Window::viewer, false, true);
	skin.saveAsObj("../out/OBJ/venus_nurbs_skin");
	Window w;
	w.launch();*/
	//Skinning* method = new PiaMethod(curves, 100);
	//Skinning* method = new NasriMethod(curves);
	//Skinning* method = new OptMethod(curves);
	PiaMinJaeMethod* method = new PiaMinJaeMethod(curves, 100);
	//PiaNasriMethod* method = new PiaNasriMethod(curves, 100);
	//Skinning* method = new MinJaeMethod(curves, 40, 10);

	method->setViewer(&Window::viewer);
	MatrixXd helper_points;
	loadpoints("../out/points/helper_points.dat", helper_points);
	//method->set_helper_points(helper_points);

	method->calculate();
	Mesh3d* mesh = &(method->tspline);
	cout << "num of nodes: " << mesh->get_num() << endl;

	mesh->saveMesh("../out/tspline/venus_skinning1");
	//mesh->saveAsObj("../out/OBJ/venus_PiaMinJae_sample_400", 0.01);
	MeshRender render(mesh,false,true,true,0.01);
	render.launch();
}

void Test::test_Bsurface_skinning()
{
	NURBSSurface surface;


	surface.loadNURBS("../out/nurbs/Bsurface_standard.cpt");
	//surface.draw(Window::viewer, false, true);
	const double resolution = 0.1;
	const int curves_num = 1.0 / resolution + 1;
	vector<NURBSCurve> curves(curves_num);

	uniform_int_distribution<unsigned> u(15, 30);
	default_random_engine e;
	default_random_engine e1;
	uniform_real_distribution<double> u1(0, 1);

	for (int i = 0; i < curves_num; i++) {
		int sampleNum = u(e);
		cout << i << " sample " << sampleNum << " points" << endl;
		Eigen::MatrixXd points(sampleNum + 2, 3);
		double z = i*resolution;

		vector<double> x_coors;
		x_coors.push_back(0);
		x_coors.push_back(1);

		for (int j = 0; j < sampleNum; j++) {
			double x = 1.0*(j + 1) / (sampleNum + 1);
			//double x = u1(e1);
			//cout << "x: " << x << endl;
			x_coors.push_back(x);
		}
		sort(x_coors.begin(), x_coors.end());
		for (int j = 0; j < x_coors.size(); j++) {
			//points.row(j) = surface.eval(x_coors[j], z);
			points.row(j) = surface.eval(z, x_coors[j]);
		}
		//Window::viewer.data().add_points(points, blue);
		curves[i].interpolate(points);
		curves[i].saveNURBS("../out/nurbs/Bsurface_curve_" + to_string(i));
		curves[i].draw(Window::viewer, false, true, 0.001);
	}

	/*Window w;
	w.launch();*/
	//Skinning* method = new PiaMethod(curves, 100);
	//Skinning* method = new NasriMethod(curves);
	//Skinning* method = new OptMethod(curves);
	//Skinning* method = new PiaMinJaeMethod(curves, 20);
	//PiaNasriMethod* method = new PiaNasriMethod(curves, 100);
	Skinning* method = new MinJaeMethod(curves, 40, 10);

	method->setViewer(&Window::viewer);

	method->calculate();
	Mesh3d* mesh = &(method->tspline);
	cout << "num of nodes: " << mesh->get_num() << endl;

	mesh->saveMesh("../out/tspline/Bsurface_skinning1");
	mesh->saveAsObj("../out/OBJ/Bsurface_MinJae_20", 0.01);
	MeshRender render(mesh, false, false, true, 0.01);
	render.launch();
}

void Test::test_fitbsplinesolid()
{
	string modelname = "tooth";
	string filename = "../out/volume/" + modelname + "_bspline.txt";
	BsplineVolume volume;
	volume.readVolume(filename);


	int sample_num = 5;
	vector<NURBSSurface> nurbs(sample_num + 1);
	MatrixXd controlpoints; // ������ʾʱ�������λ�ú����Ŵ�С
							// ��������B��������
	for (int i = 0; i <= sample_num; i++) {
		double param = 1.0*i / sample_num;
		volume.get_isoparam_surface(nurbs[i], param, 'v');
		//nurbs[i].draw(Window::viewer, false, true);
		// 
		for (int j = 0; j < nurbs[i].controlPw.size(); j++) {
			int lastid = controlpoints.rows();
			controlpoints.conservativeResize(controlpoints.rows() + nurbs[i].controlPw[j].rows(), 3);
			for (int k = 0; k < nurbs[i].controlPw[j].rows(); k++) {
				controlpoints.row(lastid + k) = nurbs[i].controlPw[j].row(k);
			}
		}

	}

	vector<Mesh3d> tsplines(sample_num + 1);
	for (int i = 0; i <= sample_num; i++) {
		// venus_bspline.txt ---> 3e-3
		// tooth_bspline.txt ---> 3
		// isis_bspline.txt --> 5e-3, 1e-2
		//string filename_b = "../out/OBJ/" + modelname + "_nurbs_" + to_string(i);
		//nurbs[i].saveAsObj(filename_b);
		TsplineSimplify(nurbs[i], tsplines[i], 20, 3);
		cout << "number of nodes: " << tsplines[i].get_num() << endl;
		/*string filename = "../out/tspline/" + modelname + "_" + to_string(i) + ".cfg";
		tsplines[i].saveMesh(filename);*/
		tsplines[i].setViewer(&Window::viewer);
		tsplines[i].draw(false, false, true, 0.01);

		Window::viewer.data_list[tsplines[i].id].set_colors(blue);
	}

	/*for (auto& surface : nurbs) {
	surface.draw(Window::viewer, false, true);
	}*/
	for (auto& data : Window::viewer.data_list) {
		data.set_face_based(true);
		data.show_lines = false;
		data.invert_normals = true;
	}
	cout << "controlpoints: " << controlpoints.rows() << endl;
	Window::viewer.core.align_camera_center(controlpoints);

	int x_points = 21;
	int y_points = 21;
	int z_points = 8;

	VectorXd knots(sample_num + 7);
	knots(0) = 0; knots(1) = 0, knots(2) = 0;
	knots(knots.size() - 1) = 1; knots(knots.size() - 2) = 1, knots(knots.size() - 3) = 1;
	for (int i = 0; i <= sample_num; i++) {
		knots(i + 3) = 1.0 * i / sample_num;
	}
	cout << "knots for interpolate: \n" << knots << endl;

	vector<vector<NURBSCurve>> sample_curves(x_points, vector<NURBSCurve>(y_points));

	vector<FitPoint3D> fit_points;

	for (int i = 0; i <= x_points - 1; i++) {
		for (int j = 0; j <= y_points - 1; j++) {
			double u = 1.0*i / (x_points - 1);
			double v = 1.0*j / (y_points - 1);
			MatrixXd points(sample_num + 1, 3);
			for (int k = 0; k <= sample_num; k++) {
				points.row(k) = tsplines[k].eval(u, v).toVectorXd();
				FitPoint3D fit_point;
				fit_point.origin.fromVectorXd(points.row(k).transpose());
				fit_point.param[0] = u;
				fit_point.param[1] = v;
				fit_point.param[2] = 1.0 * k / sample_num;
				fit_points.push_back(fit_point);
			}

			sample_curves[i][j].interpolate(points, knots);
			cout << "finished " << i << ", " << j << endl;
		}
	}
	
	BsplineVolume bvolume;
	bvolume.control_grid = vector<vector<vector<Point3d>>>(x_points, vector<vector<Point3d>>(y_points, vector<Point3d>(z_points)));
	bvolume.constructKnotVector(x_points, y_points, z_points);
	for (int i = 0; i < x_points; i++) {
		for (int j = 0; j < y_points; j++) {
			for (int k = 0; k < z_points; k++) {
				bvolume.control_grid[i][j][k].fromVectorXd(sample_curves[i][j].controlPw.row(k).transpose());
			}
		}
	}

	double alpha = 0.3;
	double delta = 0.3;
	begin = clock();
	for (int i = 0; i < 1; i++) {
		double error = bvolume.GetSoildFiterror(fit_points, x_points, y_points, z_points, alpha, delta);
		cout << "iter: " << i << ", error: " << error << endl;
		bvolume.fitBsplineSolid(fit_points, x_points, y_points, z_points, alpha, delta);
	}
	
	bvolume.saveAsHex("../out/volume/" + modelname + "_fitbspline", 0.01);
	VolumeRender render(&bvolume, false, false, true, 0.01);
	
	render.launch();
	end = clock();
	cout << "time passed: " << (end - begin) / CLOCKS_PER_SEC << "s" << endl;
}

void Test::test_nurbscurve_interpolate_optimize()
{
	const double PI = 3.1415926;
	default_random_engine e;
	uniform_real_distribution<double> u(-16 * PI, PI);
	vector<double> t;
	int sampleNum = 50;
	for (int i = 0; i <= sampleNum; i++) {
		t.push_back(1.0 * i / sampleNum * 17 * PI - 16 * PI);
		//t.push_back(u(e));
	}
	sort(t.begin(), t.end());
	MatrixXd points = MatrixXd::Zero(sampleNum + 1, 3);
	MatrixXd tangent = MatrixXd::Zero(sampleNum + 1, 3);
	for (int i = 0; i <= sampleNum; i++) {
		points(i, 0) = t[i];
		points(i, 1) = 5 * sin(t[i]);
		tangent(i, 0) = 1.0;
		tangent(i, 1) = 5 * cos(t[i]);
		tangent.row(i).normalize();
		
	}
	Window::viewer.data().add_points(points, green);
	Window::viewer.data().add_edges(points, points + tangent, blue);
	Window::viewer.core.align_camera_center(points);
	NURBSCurve curve;
	curve.interpolate_optimize(points, tangent);
	//curve.interpolate(points);
	curve.draw(Window::viewer, true, true, 0.001);
	Window w;
	w.launch();
}

/*
  ����T����������������ȷ��
*/
void Test::test_tspline_normal()
{
	Mesh3d mesh;
	mesh.loadMesh("../out/tspline/venus_0.cfg.cfg");
	for (int i = 0; i <= 10; i++) {
		for (int j = 0; j <= 10; j++) {
			double u = 1.0*i / 10;
			double v = 1.0*j / 10;

			MatrixXd point = MatrixXd::Zero(1, 3);
			point.row(0) = mesh.eval(u, v).toVectorXd();

			VectorXd normal = mesh.normal(u, v).toVectorXd() * 0.01;

			MatrixXd endpoint(1, 3);
			endpoint.row(0) = point.row(0) + normal.transpose();
			Window::viewer.data().add_points(point, blue);
			Window::viewer.data().add_edges(point, endpoint, red);
		}
		
	}
	MeshRender render(&mesh, false, true);
	render.launch();
}

/************************
�����ϻ�ȡ�Ȳ���
*************************/
void Test::test_getsurface_fromvolume()
{
	string modelname = "tooth";
	string filename = "../out/volume/" + modelname + "_bspline.txt";
	BsplineVolume volume;
	volume.readVolume(filename);


	int sample_num = 5;
	vector<NURBSSurface> nurbs(sample_num + 1);
	vector<NURBSSurface> pia_nurbs(sample_num + 1);
	MatrixXd controlpoints; // ������ʾʱ�������λ�ú����Ŵ�С
							// ��������B��������
	for (int i = 0; i <= sample_num; i++) {
		double param = 1.0*i / sample_num;
		volume.get_isoparam_surface(nurbs[i], param, 'v');
		string filename = "../out/nurbs/" + modelname + "_surface_" + to_string(i) + "_format";
		save_nurbs_surface(nurbs[i], filename);
		//NurbsPia nurbsPia(nurbs[i], 20, 1e-5, 10);
		/*pia_nurbs[i] = nurbsPia.calculate();
		if (i == 1) {
			pia_nurbs[i].draw(Window::viewer, false, true);
		}*/
	}
	Window w;
	w.launch();
	
	

}

void Test::load_nurbs_surface(NURBSSurface & surface, string filename)
{
	// ��������һ���ļ���ʽ��nurbs surface
	ifstream in(filename);
	if (!in.is_open()) {
		cout << "error: can't open file: " << filename << endl;
		return;
	}
	
	surface.isRational = false;
	surface.u_order = 4;
	surface.v_order = 4;
	surface.dimension = 3;

	string line;
	getline(in, line);
	getline(in, line);
	stringstream ss0(line);
	ss0 >> surface.v_num >> surface.u_num;
	--surface.v_num;
	--surface.u_num;

	getline(in, line);
	surface.controlPw.resize(surface.v_num + 1);
	for (int i = 0; i <= surface.v_num; i++) {
		surface.controlPw[i] = MatrixXd::Zero(surface.u_num + 1, 3);
		for (int j = 0; j <= surface.u_num; j++) {
			getline(in, line);
			istringstream ss(line);

			ss >> surface.controlPw[i](j, 0) >> surface.controlPw[i](j, 1) >> surface.controlPw[i](j, 2);
		
			//cout << surface.controlPw[i](j, 0) << ", " << surface.controlPw[i](j, 1) << ", " << surface.controlPw[i](j, 2) << endl;
		}
	}
	//cout << "***********************************************" << endl;
	getline(in, line);
	getline(in, line);
	istringstream ss1(line);
	surface.vknots = VectorXd::Zero(surface.v_num + 5);
	for (int i = 0; i < surface.vknots.size(); i++) {
		ss1 >> surface.vknots(i);
	}
	//cout << "vkonts: \n" << surface.vknots << endl;

	getline(in, line);
	getline(in, line);
	istringstream ss2(line);
	surface.uknots = VectorXd::Zero(surface.u_num + 5);
	for (int i = 0; i < surface.uknots.size(); i++) {
		ss2 >> surface.uknots(i);
	}
	//cout << "ukonts: \n" << surface.uknots << endl;
	in.close();
}

void Test::save_nurbs_surface(const NURBSSurface & surface, string filename)
{
	// ��������һ���ļ���ʽ��nurbs surface
	filename += ".cpt";
	ofstream out(filename);
	if (!out.is_open()) {
		cout << "error: can't open file: " << filename << endl;
		return;
	}
	out << "#resolution of the control grid" << endl;
	out << surface.v_num + 1 << " " << surface.u_num + 1 << endl;
	out << "#control points" << endl;
	for (int i = 0; i <= surface.v_num; i++) {
		for (int j = 0; j <= surface.u_num; j++) {
			out << surface.controlPw[i](j, 0) << " "
				<< surface.controlPw[i](j, 1) << " "
				<< surface.controlPw[i](j, 2) << endl;
		}
	}
	out << "knot vector in u-direction" << endl;
	for (int i = 0; i < surface.vknots.size(); i++) {
		out << surface.vknots(i) << " ";
	}
	out << endl;
	out << "knot vector in v-direction" << endl;
	for (int i = 0; i < surface.uknots.size(); i++) {
		out << surface.uknots(i) << " ";
	}
	out << endl;
	out.close();
}

void Test::test_load_nurbs_surface()
{
	string filename = "../out/nurbs/Bsurface.cpt";
	NURBSSurface surface;
	load_nurbs_surface(surface, filename);
	surface.draw(Window::viewer, false, true);
	surface.saveNURBS("../out/nurbs/Bsurface_standard");
	Window w;
	w.launch();
}

void Test::test_save_nurbs_surface()
{
	string filename = "../out/nurbs/Bsurface_standard.cpt";
	NURBSSurface surface;
	surface.loadNURBS(filename);
	surface.draw(Window::viewer, false, true);
	save_nurbs_surface(surface, "../out/nurbs/Bsurface1");
	Window w;
	w.launch();
}

/**
   ������Ƥ
*/
void Test::test_chess_skinning()
{
	
	vector<MatrixXd> curve_points(13);
	// ��ȡ�ļ����õ�ɢ��
	for (int i = 0; i < curve_points.size(); i++) {
		string filename = "../out/curves/selected_point" + to_string(i + 1) + ".txt";
		ifstream in(filename);
		if (!in.is_open()) {
			cout << "error : can't open file: " << filename << endl;
			return;
		}
		string line;
		char c; // ��ȡ'v'
		while (getline(in, line)) {
			curve_points[i].conservativeResize(curve_points[i].rows() + 1, 3);
			int lastid = curve_points[i].rows() - 1;
			stringstream ss(line);
			ss >> c >> curve_points[i](lastid, 0) >> curve_points[i](lastid, 1) >> curve_points[i](lastid, 2);
		}
		in.close();
	}
	
	// ��ֵ�õ�B��������
	vector<NURBSCurve> nurbs_curves(curve_points.size());
	for (int i = 0; i < curve_points.size(); i++) {
		nurbs_curves[i].interpolate(curve_points[i]);
		nurbs_curves[i].draw(Window::viewer, false, true);
		Window::viewer.data().add_points(curve_points[i], green);
	}

	Skinning* method = new PiaMinJaeMethod(nurbs_curves, 20);
	method->setViewer(&Window::viewer);
	method->calculate();
	MeshRender render(&method->tspline, false, true, true);
	render.launch();

}

/**
 ��B������������һ��B��������
 ���ں�����ΪT�������棬��һ������Volume Skinning
*/
void Test::test_generate_surfaces()
{
	string modelname = "tooth";
	string filename = "../out/volume/" + modelname + "_bspline.txt";
	BsplineVolume volume;
	volume.readVolume(filename);
	

	int sample_num = 5;
	vector<NURBSSurface> nurbs(sample_num + 1);
	MatrixXd controlpoints; // ������ʾʱ�������λ�ú����Ŵ�С
	// ��������B��������
	for (int i = 0; i <= sample_num; i++) {
		double param = 1.0*i / sample_num;
		volume.get_isoparam_surface(nurbs[i], param, 'v');
		//nurbs[i].draw(Window::viewer, false, true);
		// 
		for (int j = 0; j < nurbs[i].controlPw.size(); j++) {
			int lastid = controlpoints.rows();
			controlpoints.conservativeResize(controlpoints.rows() + nurbs[i].controlPw[j].rows(), 3);
			for (int k = 0; k < nurbs[i].controlPw[j].rows(); k++) {
				controlpoints.row(lastid + k) = nurbs[i].controlPw[j].row(k);
			}
		}

	}
	
	vector<Mesh3d> tsplines(sample_num + 1);
	for (int i = 0; i <= sample_num; i++) {
		// venus_bspline.txt ---> 3e-3
		// tooth_bspline.txt ---> 3
		// isis_bspline.txt --> 5e-3, 1e-2
		//string filename_b = "../out/OBJ/" + modelname + "_nurbs_" + to_string(i);
		//nurbs[i].saveAsObj(filename_b);
		TsplineSimplify(nurbs[i], tsplines[i], 20, 3);
		cout << "number of nodes: " << tsplines[i].get_num() << endl;
		/*string filename = "../out/tspline/" + modelname + "_" + to_string(i) + ".cfg";
		tsplines[i].saveMesh(filename);*/
		tsplines[i].setViewer(&Window::viewer);
		tsplines[i].draw(false, false, true, 0.01);

		Window::viewer.data_list[tsplines[i].id].set_colors(blue);
	}
	
	/*for (auto& surface : nurbs) {
		surface.draw(Window::viewer, false, true);
	}*/
	for (auto& data : Window::viewer.data_list) {
		data.set_face_based(true);
		data.show_lines = false;
		data.invert_normals = true;
	}
	cout << "controlpoints: " << controlpoints.rows() << endl;
	Window::viewer.core.align_camera_center(controlpoints);

	VolumeSkinning* method = new VolumePiaMethod(tsplines, 12, 1e-5);
	//VolumeSkinning* method = new VolumeSkinning(tsplines);
	method->setViewer(&Window::viewer);
	method->calculate();
	method->volume.saveVolume("../out/volume/" + modelname + "_skinning");
	method->volume.saveAsHex("../out/volume/" + modelname + "_skinning", 0.01);
	VolumeRender render(&method->volume, false, false, true, 0.01);
	begin = clock();
	render.launch();
	end = clock();
	cout << "time for drawing tspline volume: " << (end - begin) / CLOCKS_PER_SEC << "s" << endl;
	/*Window w;
	w.launch();*/
	
}

void Test::test_nurbs()
{
	// bezier curve
	/*NURBSCurve nurbs;
	nurbs.loadNURBS("../out/nurbs/bezier.cpt");
	nurbs.draw(Window::viewer);*/

	// bezier surface
	/*NURBSSurface nurbs;
	nurbs.loadNURBS("../out/nurbs/beziersurface.cpt");
	nurbs.draw(Window::viewer);*/

	// uniform bspline cruve
	NURBSCurve nurbs;
	nurbs.loadNURBS("../out/nurbs/bsplinecurve.cpt");
	nurbs.draw(Window::viewer);

	// uniform bspline surface 
	/*NURBSSurface nurbs;
	nurbs.loadNURBS("../out/nurbs/bsplinesurface.cpt");
	nurbs.draw(Window::viewer);*/

	// nurbs surface
	/*NURBSSurface nurbs;
	nurbs.loadNURBS("../out/nurbs/torus.cptw");
	nurbs.draw(Window::viewer);*/

	for (int i = 0; i <= 10; i++) {
		MatrixXd point = nurbs.eval(1.0 * i / 10);
		MatrixXd tangent = nurbs.eval_tangent(1.0 * i / 10);
		Window::viewer.data().add_edges(point, point + tangent, red);
	}
	Window w;
	w.launch();

	
}

void Test::test_TsplineVolume() {
	TsplineVolume* volume = new TsplineVolume();
	volume->readVolume("../out/volume/tooth_skinning.vol");
	
	VolumeRender render(volume, false, false, true, 0.01);
	begin = clock();
	render.launch();
	end = clock();
	cout << "time for drawing tspline volume: " << (end - begin) / CLOCKS_PER_SEC << "s" << endl;
}
void Test::test_BsplineVolume()
{
	BsplineVolume volume;
	begin = clock();
	volume.readVolume("../out/volume/venus_bspline.txt");
	volume.setReverse(true);
	//volume.readVolume("../out/volume/tooth_bspline.txt");
	//volume.readVolume("../out/volume/balljoint_bspline.txt");
	//volume.readVolume("../out/volume/isis_bspline.txt");
	//volume.readVolume("../out/volume/moai_bspline.txt");
	//volume.readVolume("../out/volume/tooth_bspline.txt");
	volume.saveAsHex("../out/volume/venus_volume", 0.01);
	VolumeRender render(&volume, false, false, true, 0.01);
	/*volume.saveAsHex("../out/volume/tooth_bspline", 0.1);
	volume.saveVolume("../out/volume/tooth_bspline");*/
	
	
	render.launch();
	end = clock();
	cout << "time passed: " << (end - begin) / CLOCKS_PER_SEC << "s" << endl;
}
void Test::test_Mesh() {

	//Mesh3d* mesh = new Mesh3d();
	//mesh->loadMesh("../out/tspline/simpleMesh2.cfg");
	//Mesh3d* meshcopy = new Mesh3d(*mesh); // deep copy
	//meshcopy->saveMesh("../out/tspline/simpleMesh2_copy");
	//delete mesh;
	//mesh = NULL;
	//MeshRender render(meshcopy);
	//render.launch();
	Mesh3d mesh;
	mesh.loadMesh("../out/tspline/venus_1.cfg.cfg");
	mesh.saveAsObj("../out/tspline/venus_1");
	MeshRender render(&mesh, false, true, true);
	begin = clock();
	render.launch();
	end = clock();
	cout << "time for drawing tspline: " << (end - begin) / CLOCKS_PER_SEC << "s" << endl;

}

void Test::test_VolumeSkinning()
{
	/*string modelname = "venus";
	string prefix = "../out/nurbs/venus_surface_";*/

	string modelname = "tooth";
	string prefix = "../out/nurbs/tooth_";

	int sample_num = 5;
	vector<NURBSSurface> nurbs(sample_num + 1);
	for (int i = 0; i <= sample_num; i++) {
		//string filename = prefix + to_string(i) + "_format.cpt";
		string filename = prefix + to_string(i) + ".cpt";
		load_nurbs_surface(nurbs[i], filename);
	}
	MatrixXd controlpoints; // ������ʾʱ�������λ�ú����Ŵ�С
							// ��������B��������
	for (int i = 0; i <= sample_num; i++) {

		for (int j = 0; j < nurbs[i].controlPw.size(); j++) {
			int lastid = controlpoints.rows();
			controlpoints.conservativeResize(controlpoints.rows() + nurbs[i].controlPw[j].rows(), 3);
			for (int k = 0; k < nurbs[i].controlPw[j].rows(); k++) {
				controlpoints.row(lastid + k) = nurbs[i].controlPw[j].row(k);
			}
		}

	}

	vector<Mesh3d> tsplines(sample_num + 1);
	for (int i = 0; i <= sample_num; i++) {
		// venus_bspline.txt ---> 3e-3
		// tooth_bspline.txt ---> 3
		// isis_bspline.txt --> 5e-3, 1e-2
		//string filename_b = "../out/OBJ/" + modelname + "_nurbs_" + to_string(i);
		//nurbs[i].saveAsObj(filename_b);
		TsplineSimplify(nurbs[i], tsplines[i], 20, 3);
		cout << "number of nodes: " << tsplines[i].get_num() << endl;
		/*string filename = "../out/tspline/" + modelname + "_" + to_string(i) + ".cfg";
		tsplines[i].saveMesh(filename);*/
		tsplines[i].setViewer(&Window::viewer);
		tsplines[i].draw(false, false, true, 0.01);

		Window::viewer.data_list[tsplines[i].id].set_colors(blue);
	}

	/*for (auto& surface : nurbs) {
	surface.draw(Window::viewer, false, true);
	}*/
	for (auto& data : Window::viewer.data_list) {
		data.set_face_based(true);
		data.show_lines = false;
		data.invert_normals = true;
	}
	cout << "controlpoints: " << controlpoints.rows() << endl;
	Window::viewer.core.align_camera_center(controlpoints);

	VolumeSkinning* method = new VolumePiaMethod(tsplines, 12, 1e-5);
	//VolumeSkinning* method = new VolumeSkinning(tsplines);
	method->setViewer(&Window::viewer);
	method->calculate();
	method->volume.saveVolume("../out/volume/" + modelname + "_skinning");
	method->volume.saveAsHex("../out/volume/" + modelname + "_skinning", 0.01);
	VolumeRender render(&method->volume, false, false, true, 0.01);
	begin = clock();
	render.launch();
	end = clock();
	cout << "time for drawing tspline volume: " << (end - begin) / CLOCKS_PER_SEC << "s" << endl;
	/*Window w;
	w.launch();*/
}

void Test::test_circle_skinning()
{

	vector<NURBSCurve> nurbs(4);
	nurbs[0].loadNURBS("../out/nurbs/circle.cptw");
	nurbs[1].loadNURBS("../out/nurbs/circle1_1.cptw");
	nurbs[2].loadNURBS("../out/nurbs/circle2_1.cptw");
	nurbs[3].loadNURBS("../out/nurbs/circle3.cptw");

	nurbs[0].draw(Window::viewer, false);
	nurbs[1].draw(Window::viewer, false);
	nurbs[2].draw(Window::viewer, false);
	nurbs[3].draw(Window::viewer, false);
	
	//Skinning* method = new MinJaeMethod(nurbs, 20, 50);
	//Skinning* method = new PiaMethod(nurbs, 100);
	//Skinning* method = new NasriMethod(nurbs);
	//Skinning* method = new OptMethod(nurbs);
	Skinning* method = new PiaMinJaeMethod(nurbs, 20);
	//Skinning* method = new PiaNasriMethod(nurbs, 100);

	method->setViewer(&Window::viewer);
	method->calculate();
	Mesh3d* mesh = &(method->tspline);
	cout << "num of nodes: " << mesh->get_num() << endl;
	
	//mesh->saveMesh("../out/tspline/simpleMesh4");
	MeshRender render(mesh);
	render.launch();


}
void Test::test_DerOfNurbs() {
	NURBSCurve nurbs;
	nurbs.loadNURBS("../out/nurbs/circle.cptw");
	cout << "controlpw: \n" << nurbs.controlPw << endl;
	for (int i = 0; i <= 10; i++) {
		double u = 1.0*i / 10;
		MatrixXd point = MatrixXd::Zero(1, 3);
		point.row(0) = nurbs.eval(u);
		RowVector3d du = RowVector3d::Zero(); // Ĭ�ϲ���0

		for (int j = 0; j <= nurbs.n; j++) {
			double a = t_mesh::DersBasis(nurbs.knots, u, j, 3)(1);
			if (i == 0) {
				cout << "a: " << a << endl;
			}
			du += nurbs.controlPw.row(j) * a;
			if (i == 0) {
				cout << "du: \n" << du << endl;
			}
		}
		if (i == 0) {
			cout << "du: \n" << du << endl;
		}
		du.normalize();
		if (i == 0) {
			cout << "du: \n" << du << endl;
		}
		MatrixXd endpoint(1, 3);
		endpoint.row(0) = point.row(0) + du;
		Window::viewer.data().add_points(point, blue);
		Window::viewer.data().add_edges(point, endpoint, green);
	}
	nurbs.draw(Window::viewer);
	Window w;
	w.launch();
}
void Test::test_Lspia() {
	MatrixXd points;
	t_mesh::loadpoints("../out/points/helix.dat", points);
	
	NURBSCurve fit;
	fit.lspiafit(points, 10);

	fit.draw(Window::viewer, true,true,0.001);

	Window::viewer.data().add_points(points, red);

	Window w;
	w.launch();
}
void Test::test_Array() {
	t_mesh::Array<double, 5> A;
	A.input(cin);
	A = 2.0*A;
	A.output(cout);
	cout << endl;
	A = A * 2.0;
	A.output(cout);
	cout << endl;
}
void Test::test_Integral() {
	double a = 2.0;
	auto lambda = [a](double u, double v)-> double {
		return a*sin(u + v);
	};
	double res = OptMethod::integral(lambda);
	cout << "ingegral: " << res << endl;
	cout << "real: " << 2 * (2 * sin(1) - sin(2)) << endl;
}

void Test::test_Basis() {

	t_mesh::Array<double, 5> A;
	A.input(cin);
	A.output(cout);
	double t = 0.0;
	cin >> t;

	cout << "basis: " << t_mesh::Basis(A.toVectorXd(), t) << endl;
	cout << "basis1: " << Basis1(A.toVectorXd(), t) << endl;
}
void Test::test_Derivative() {
	Eigen::VectorXd knots(13);
	knots << 0, 0, 0, 0, 1, 2, 3, 4, 4, 5, 5, 5, 5;
	double t = 5;
	cout << "derivative: \n" << DersBasis(knots, t, 8, 3) << endl;

	t_mesh::Array<double, 5> A;
	A.input(cin);
	A.output(cout);
	t = 0.0;
	cin >> t;

	cout << "derivative basis: \n" << NURBSCurve::DersBasis(A.toVectorXd(), t) << endl;

}
double Test::myfunc(const std::vector<double> &x, std::vector<double> &grad, void *my_func_data)
{
	if (!grad.empty()) {
		grad[0] = 0.0;
		grad[1] = 0.5 / sqrt(x[1]);
	}
	return sqrt(x[1]);
}

double Test::myconstraint(const std::vector<double> &x, std::vector<double> &grad, void *data)
{
	my_constraint_data *d = reinterpret_cast<my_constraint_data*>(data);
	double a = d->a, b = d->b;
	if (!grad.empty()) {
		grad[0] = 3 * a * (a*x[0] + b) * (a*x[0] + b);
		grad[1] = -1.0;
	}
	return ((a*x[0] + b) * (a*x[0] + b) * (a*x[0] + b) - x[1]);
}

void Test::test_Nlopt() {
	nlopt::opt opt(nlopt::LD_MMA, 2);
	std::vector<double> lb(2);
	lb[0] = -HUGE_VAL; lb[1] = 0;
	opt.set_lower_bounds(lb);
	opt.set_min_objective(myfunc, NULL);
	my_constraint_data data[2] = { { 2,0 },{ -1,1 } };
	opt.add_inequality_constraint(myconstraint, &data[0], 1e-8);
	opt.add_inequality_constraint(myconstraint, &data[1], 1e-8);
	opt.set_xtol_rel(1e-4);
	std::vector<double> x(2);
	x[0] = 1.234; x[1] = 5.678;
	double minf;

	try {
		nlopt::result result = opt.optimize(x, minf);
		std::cout << "found minimum at f(" << x[0] << "," << x[1] << ") = "
			<< std::setprecision(10) << minf << std::endl;
	}
	catch (std::exception &e) {
		std::cout << "nlopt failed: " << e.what() << std::endl;
	}
}