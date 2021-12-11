#include<iostream>
#include "./ResourceScheduler.h"
#include "./Util.h"

int main() {
	int taskType = 2;
	int caseID = 1;
	ResourceScheduler rs(taskType, caseID, 0);// 0 random; 1 file;
	rs.Initial();
	//generator(rs,taskType);
	if (taskType == 1) {
		rs.scheduleDeng();
	}
	else if (taskType == 2) {
		ResourceScheduler origin = rs;
		rs.transferToHost0();
		rs.scheduleDeng();
		rs.resolveFromHost0(origin);
		rs.adjustTime();

	}
	

	//rs.schedule();
	rs.outputSolutionFromBlock();
	rs.outputSolutionFromCore();
	//rs.printhostCoreTask();

	return 0;
}
