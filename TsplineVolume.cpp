#include "TsplineVolume.h"


TsplineVolume::TsplineVolume(const TsplineVolume & other)
{
	for (auto entry : other.w_map) {
		Mesh3d* mesh = new Mesh3d(*(entry.second));
		this->w_map[entry.first] = mesh;
	}
	this->w_knots = other.w_knots;
}

TsplineVolume::~TsplineVolume()
{
	for (auto entry : w_map) {
		delete entry.second;
		entry.second = NULL;
	}
}

Point3d TsplineVolume::eval(double u, double v, double w)
{
	Point3d res;
	int i = 0;
	for (auto w_it = w_map.begin(); w_it != w_map.end(); w_it++,i++) {
		Mesh3d* tmesh = w_it->second;
		double w_basis = Basis(w_knots, w, i);
		res.add(w_basis * tmesh->eval(u, v));
	}
	return res;
}

int TsplineVolume::readVolume(string filename)
{
	ifstream in(filename);
	if (!in.is_open()) {
		cout << "failed to open file: " << filename << endl;
		return -1;
	}
	int num = 0;
	in >> num;
	w_knots = Eigen::VectorXd::Zero(num);
	for (int i = 0; i < num; i++) {
		double temp = 0;
		in >> temp;
		w_knots(i) = temp;
	}
	for (int i = 0; i < num - 4; i++) {
		Mesh3d* tmesh = new Mesh3d();
		tmesh->loadMesh(in);
		w_map[w_knots(i + 2)] = tmesh;
	}

	return 0;
}

int TsplineVolume::saveVolume(string filename)
{
	ofstream out(filename + ".vol");
	if (!out.is_open()) {
		cout << "failed to create or open file: " << filename <<".vol"<< endl;
		return -1;
	}
	out << w_knots.size() << endl;
	for (int i = 0; i < w_knots.size(); i++) {
		out << w_knots(i) << " ";
	}
	out << endl;
	for (auto entry : w_map) {
		entry.second->saveMesh(out);
	}
	return 0;
	
}

void TsplineVolume::drawTmesh()
{
	assert(viewer != NULL); // use setViewer(Viewer* viewer)

	(*viewer).data().add_label(Eigen::Vector3d(-0.02, -0.02, -0.02), "O");
	(*viewer).data().add_label(Eigen::Vector3d(1.05, 0, 0), "u");
	(*viewer).data().add_label(Eigen::Vector3d(0, 1.05, 0), "v");
	(*viewer).data().add_label(Eigen::Vector3d(0, 0, 1.05), "w");

	Eigen::MatrixXd points;

	for (auto w_it = w_map.begin(); w_it != w_map.end(); w_it++) {
		double w = w_it->first;
		Mesh3d* tmesh = w_it->second;
		

		auto nodes = tmesh->nodes;

		// nodes of one layer
		for (int i = 0; i < nodes.size(); i++) {
			Eigen::MatrixXd point(1, 3);
			point << nodes[i]->s[2], nodes[i]->t[2], w;

			(*viewer).data().add_points(point, red);

			points.conservativeResize(point.rows() + 1, 3);
			points.row(points.rows() - 1) = point;

			// ������һ������ͬ��(u,v)�ڵ㣬�����ӳɱ�
			if (next(w_it) != w_map.end()) {
				double w_next = next(w_it)->first; // ��һ��� w ֵ
				Mesh3d* tmesh_next = next(w_it)->second; // ��һ�������
				auto node_next = tmesh_next->get_node(point(0, 0), point(0, 1));
				if (node_next != NULL) {
					Eigen::MatrixXd point_next = point;
					point_next(0, 2) = w_next;
					(*viewer).data().add_edges(point, point_next, white);
				}
			}
			
		}
		Eigen::MatrixXd P1 = Eigen::MatrixXd::Zero(1, 3);
		Eigen::MatrixXd P2 = Eigen::MatrixXd::Zero(1, 3);
		P1(0, 2) = w;
		P2(0, 2) = w;
		for (auto iter = tmesh->s_map.begin(); iter != tmesh->s_map.end(); ++iter) {
			for (auto iter1 = (iter->second).begin(); iter1 != (iter->second).end(); ++iter1) {
				if ((iter1->second)->adj[2]) {
					P1(0, 0) = iter->first;
					P1(0, 1) = iter1->first;
					P2(0, 0) = iter->first;
					P2(0, 1) = (iter1->second)->adj[2]->t[2];
					(*viewer).data().add_edges(P1, P2, white);
				}
			}
		}

		for (auto iter = tmesh->t_map.begin(); iter != tmesh->t_map.end(); ++iter) {
			for (auto iter1 = (iter->second).begin(); iter1 != (iter->second).end(); ++iter1) {
				if ((iter1->second)->adj[1]) {
					P1(0, 0) = iter1->first;
					P1(0, 1) = iter->first;
					P2(0, 0) = (iter1->second)->adj[1]->s[2];
					P2(0, 1) = iter->first;
					(*viewer).data().add_edges(P1, P2, white);
				}
			}
		}
	}
	(*viewer).core.align_camera_center(points); // center
}

void TsplineVolume::drawControlpolygon()
{
	assert(viewer != NULL); // use setViewer(Viewer* viewer)
	Eigen::MatrixXd points;

	for (auto w_it = w_map.begin(); w_it != w_map.end(); w_it++) {
		double w = w_it->first;
		Mesh3d* tmesh = w_it->second;
		auto nodes = tmesh->nodes;

		// control points of one layer
		for (int i = 0; i < nodes.size(); i++) {
			Eigen::MatrixXd point(1, 3);
			array2matrixd(nodes[i]->data, point);

			(*viewer).data().add_points(point, red);
			points.conservativeResize(points.rows() + 1, 3);
			points.row(points.rows() - 1) = point;

			// ������һ������ͬ��(u,v)�ڵ㣬�����ӳɱ�
			if (next(w_it) != w_map.end()) {
				double w_next = next(w_it)->first; // ��һ��� w ֵ
				Mesh3d* tmesh_next = next(w_it)->second; // ��һ�������
				auto node_next = tmesh_next->get_node(nodes[i]->s[2], nodes[i]->t[2]);
				if (node_next != NULL) {
					Eigen::MatrixXd point_next;
					array2matrixd(node_next->data, point_next);
					(*viewer).data().add_edges(point, point_next, yellow);
				}
			}
		}

		Eigen::MatrixXd P1, P2;

		for (auto iter = tmesh->s_map.begin(); iter != tmesh->s_map.end(); ++iter) {
			for (auto iter1 = (iter->second).begin(); iter1 != (iter->second).end(); ++iter1) {
				if ((iter1->second)->adj[2]) {
					array2matrixd(iter1->second->data, P1);
					array2matrixd((iter1->second->adj[2])->data, P2);
					(*viewer).data().add_edges(P1, P2, green);
				}
			}
		}

		for (auto iter = tmesh->t_map.begin(); iter != tmesh->t_map.end(); ++iter) {
			for (auto iter1 = (iter->second).begin(); iter1 != (iter->second).end(); ++iter1) {
				if ((iter1->second)->adj[1]) {
					array2matrixd(iter1->second->data, P1);
					array2matrixd((iter1->second->adj[1])->data, P2);
					(*viewer).data().add_edges(P1, P2, blue);
				}
			}
		}
		
	}
	(*viewer).core.align_camera_center(points);
	

}