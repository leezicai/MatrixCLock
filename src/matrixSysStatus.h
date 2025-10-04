#ifndef MATRIXSYSSTATUS_H
#define MATRIXSYSSTATUS_H

#include "matrixSetting.h"

class MatrixStatusManager {
	private:
		char intBuffer[12]; 
	public:
	 	MatrixStatusManager();
		const char* getSysStatus(int index);

};

extern MatrixStatusManager matrixStatusManager;

#endif