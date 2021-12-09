#include<iostream>
#include "ResourceScheduler.h"
#include "Util.h"

int main() {
	int taskType=1;
	int caseID=1;
	
	ResourceScheduler rs(taskType,caseID);
	generator(rs, taskType);
	rs.JobSchInit();
	
	//rs.scheduleTwoStep2();
	cout << endl;
	rs.scheduleTwoStep();
	//rs.schedule();
	//rs.outputSolutionFromBlock();
	//rs.outputSolutionFromCore();

	return 0;
}

