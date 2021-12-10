#include<iostream>
#include "./ResourceScheduler.h"
#include "./Util.h"

int main() {
	int taskType = 2;
	int caseID = 1;
	ResourceScheduler rs(taskType, caseID, 1); // 0 random; 1 file;
	rs.Initial();
	//generator(rs,taskType);
	if (taskType == 1) {
		rs.scheduleDeng();
		rs.scheduleTwoStep();
		rs.scheduleTwoStep2();
	}
	else if (taskType == 2) {
		ResourceScheduler origin = rs;
		rs.transferToHost0();
		rs.scheduleDeng();

	}
	

	//rs.schedule();
	rs.outputSolutionFromBlock();
	rs.outputSolutionFromCore();
	//rs.scheduleDeng();

	return 0;
}
