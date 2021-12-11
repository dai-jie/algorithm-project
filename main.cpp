#include<iostream>
#include "./ResourceScheduler.h"
#include "./Util.h"


int main() {
	int taskType = 1;
	int caseID = 1;

	vector<pair<double, double>> beta;

	for (int i = 0; i < 100; i++)
	{
		double average = 0;
		double para_alpha = 0.01 + i * 0.0005;
		cout << para_alpha << " ";
		for (size_t i = 0; i < 100; i++)
		{
			ResourceScheduler rs(taskType, caseID, 0, para_alpha);// 0 random; 1 file;
			rs.Initial();



			if (taskType == 1) {
				rs.scheduleDeng();
				rs.outputSolutionFromCore();
				average += rs.utilization;
			}
			else if (taskType == 2) {
				ResourceScheduler origin = rs;
				rs.transferToHost0();
				rs.scheduleDeng();
				rs.resolveFromHost0(origin);
				rs.adjustTime();
				rs.outputSolutionFromCore();
				average += rs.utilization;
			}
		}
		average /= 100;
		cout << average << endl;
	}
	//ResourceScheduler rs(taskType, caseID, 0);// 0 random; 1 file;
	//rs.Initial();

	//if (taskType == 1) {
	//	rs.scheduleDeng();
	//}
	//else if (taskType == 2) {
	//	ResourceScheduler origin = rs;
	//	rs.transferToHost0();
	//	rs.scheduleDeng();
	//	rs.resolveFromHost0(origin);
	//	rs.adjustTime();

	//}
	

	//rs.schedule();
	//rs.outputSolutionFromBlock();
	//rs.outputSolutionFromCore();
	return 0;
}
