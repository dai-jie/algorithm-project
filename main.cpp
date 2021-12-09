#include<iostream>
//#include "../hFiles/ResourceScheduler.h"
#include "./ResourceScheduler.h"
//#include "../hFiles/Util.h"
#include "./Util.h"
#include<filesystem>

int main() {
	int taskType = 1;
	int caseID = 1;
	cout << std::filesystem::current_path();
	ResourceScheduler rs(taskType, caseID);
	//generator(rs,taskType);
	rs.Initial();
	rs.scheduleTwoStep();
	rs.scheduleTwoStep2();

	//rs.schedule();
	rs.outputSolutionFromBlock();
	rs.outputSolutionFromCore();

	return 0;
}
