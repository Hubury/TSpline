#include "skinning.h"
class OptMethod :public Skinning {
	OptMethod(const vector<NURBSCurve>& _curves):Skinning(_curves){

	}

	void init() override;		// ����NUUBSCurve��ʼ��T-preimage
	void insert() override;		// ��T-preimage�в����м�ڵ�
	void calculate() override;  // ��������
};