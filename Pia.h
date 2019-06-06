#include "FitPoint.hpp"
#include "utility.h"
using namespace std;
using namespace t_mesh;
class Pia{
public:
	Pia(std::vector<FitPoint2D> _fitPoints, int _maxIterNum = 100, double _eps = 1e-5)
		:fitPoints(_fitPoints),maxIterNum(_maxIterNum), eps(_eps) {

	}
	void init();		// ����NUUBSCurve��ʼ��T-preimage
	void adapative_insert();		// ��һ������������ĵط�����ڵ㣬�ֲ���ϸ
	void calculate();           // ��������

	void fit();
	void pia();
public:
	Mesh3d tspline;
private:
	const int maxIterNum;
	const double eps;
	double error;
	vector<FitPoint2D> fitPoints;
};