#ifndef MATRIXSYSSTATUS_H
#define MATRIXSYSSTATUS_H

#include "matrixSetting.h"
#include "ds3231.h"
#include "sht30.h"
#include "matrixDma.h"

class MatrixStatusManager {
	private:
		char intBuffer[32]; 
	public:
	 	MatrixStatusManager();
		const char* getSysStatus(int index);

};

extern MatrixStatusManager matrixStatusManager;

#endif