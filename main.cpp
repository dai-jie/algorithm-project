#include<iostream>
#include "./ResourceScheduler.h"
#include "./Util.h"

int main() {
	int taskType = 1;
	int caseID = 1;
	ResourceScheduler rs(taskType, caseID, 1); // 0 random; 1 file;
	//generator(rs,taskType);
	if (taskType == 1) {
		rs.Initial();
		rs.scheduleDeng();
		rs.scheduleTwoStep();
		rs.scheduleTwoStep2();
	}
	else if (taskType == 2) {
		ResourceScheduler origin = rs;
		rs.transferToHost0();

	}
	

	//rs.schedule();
	rs.outputSolutionFromBlock();
	rs.outputSolutionFromCore();
	//rs.scheduleDeng();

	return 0;
}
