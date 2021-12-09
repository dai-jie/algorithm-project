//#include "../hFiles/ResourceScheduler.h"
#include "ResourceScheduler.h"
#include <random>


ResourceScheduler::ResourceScheduler(int tasktype, int caseID) {
	taskType = tasktype;
	srand((int)time(0));
	numJob = 10, numHost = 1, alpha = 0.01;
	if (taskType == 2) St = 500;
	int minCore = 3, maxCore = 20;
	//int minCore = 3, maxCore = 20;
	int minBlock = 20, maxBlock = 500;
	int minSize = 50, maxSize = 200;
	double minSpeed = 20, maxSpeed = 80;
	hostCore.resize(numHost);
	jobBlock.resize(numJob);
	Sc.resize(numJob);
	dataSize.resize(numJob);
	location.resize(numJob);

	cout << "\n\n-----------Generator starts.--------------\n\n";

	cout << "numJob = " << numJob << ", numHost = " << numHost << ", alpha = " << alpha;
	if (taskType == 2) cout << ", St = " << St;
	cout << "\n\n";

	cout << "hostCore:\n";
	for (int i = 0; i < numHost; i++) {
		do {
			hostCore[i] = rand() % (maxCore - minCore + 1) + minCore;
			//cout << hostCore[i] << endl;
		} while (alpha >= 1.0 / (hostCore[i] - 1));

		cout << hostCore[i] << " ";
	}

	cout << "\n\njobBlockNumber:\n";
	for (int i = 0; i < numJob; i++) {
		jobBlock[i] = rand() % (maxBlock - minBlock + 1) + minBlock;
		cout << jobBlock[i] << " ";
	}

	cout << "\n\njobCalculatingSpeed:\n";
	for (int i = 0; i < numJob; i++)
	{
		Sc[i] = rand() % int(maxSpeed - minSpeed + 1) + minSpeed;
		cout << Sc[i] << " ";
	}

	cout << "\n\nblockDataSize:\n";
	for (int i = 0; i < numJob; i++)
	{
		dataSize[i].resize(jobBlock[i]);
		for (int j = 0; j < jobBlock[i]; j++) {
			dataSize[i][j] = rand() % (maxSize - minSize + 1) + minSize;
			//dataSize[i][j] = fabs(std::normal_distribution<double> n(4, 1.5));
			cout << dataSize[i][j] << " ";
		}
		cout << endl;
	}

	cout << "\njobBlockInitialLocation:\n";
	for (int i = 0; i < numJob; i++)
	{
		location[i].resize(jobBlock[i]);
		for (int j = 0; j < jobBlock[i]; j++) {
			location[i][j] = rand() % numHost;
			cout << location[i][j] << " ";
		}
		cout << endl;
	}

	jobFinishTime.resize(numJob, 0);
	jobCore.resize(numJob);

	runLoc.resize(numJob);
	for (int i = 0; i < numJob; i++)
		runLoc[i].resize(jobBlock[i]);

	hostCoreTask.resize(numHost);
	for (int i = 0; i < numHost; i++)
		hostCoreTask[i].resize(hostCore[i]);

	hostCoreFinishTime.resize(numHost);
	for (int i = 0; i < numHost; i++)
		hostCoreFinishTime[i].resize(hostCore[i], 0);

	cout << "\n\n-----------Generator ends.--------------\n\n";
}

void ResourceScheduler::Initial() {
	blockTime.resize(numJob);
	for (int i = 0; i < numJob; i++) {
		blockTime[i].resize(hostCore[0]);
		for (int j = 0; j < hostCore[0]; j++) {
			blockTime[i][j].resize(jobBlock[i]);
			for (int k = 0; k < jobBlock[i]; k++) {
				blockTime[i][j][k] = dataSize[i][k] / (g(j + 1) * Sc[i]);
			}
		}

	}

	/*for (int i = 0; i < numJob; i++) {
		;
		for (int j = 0; j < hostCore[0]; j++) {

			for (int k = 0; k < jobBlock[i]; k++) {
				cout << blockTime[i][j][k] << " ";
			}
			cout << endl;
		}

	}*/

	blockSch.resize(numJob);
	for (int i = 0; i < numJob; i++)
	{
		blockSch[i].resize(hostCore[0]);
		for (int j = 0; j < hostCore[0]; j++)
		{
			blockSch[i][j].resize(j + 1, 0);
		}
	}

	for (int i = 0; i < numJob; i++)
	{
		for (int j = 0; j < hostCore[0]; j++)
		{
			int coreNum = j + 1;
			priority_queue<double, vector<double>, greater<double> > timeQueue;
			for (int k = 0; k < coreNum; k++) timeQueue.push(0.0);
			sort(blockTime[i][j].begin(), blockTime[i][j].end(), greater<double>());
			for (int k = 0; k < jobBlock[i]; k++)
			{
				double finishTime = timeQueue.top() + blockTime[i][j][k];
				timeQueue.pop();
				timeQueue.push(finishTime);
			}
			for (int k = 0; k < coreNum; k++)
			{
				blockSch[i][j][k] = timeQueue.top();
				timeQueue.pop();
			}

		}
	}

	/*for (int i = 0; i < numJob; i++)
	{
		for (int j = 0; j < hostCore[0]; j++)
		{
			for (int k = 0; k < blockSch[i][j].size(); k++)
			{
				cout << blockSch[i][j][k] << " ";

			}
			cout << endl;

		}
	}*/


}

void ResourceScheduler::schedule() {

	vector<vector<int>> hostCoreBlock(numHost);
	for (int i = 0; i < numHost; i++)
		hostCoreBlock[i].resize(hostCore[i], 0);

	for (int i = 0; i < numJob; i++) {
		set<pair<int, int>> allocatedJobCore;
		for (int j = 0; j < jobBlock[i]; j++) {
			int hid = rand() % numHost;
			int cid = rand() % hostCore[hid];
			allocatedJobCore.insert({ hid,cid });
			runLoc[i][j] = make_tuple(hid, cid, hostCoreBlock[hid][cid]++);
		}
		/*for (int j = 0; j < jobBlock[i]; j++)
			finishTime[i]+=*/
		jobFinishTime[i] = rand() % 200;
		jobCore[i] = allocatedJobCore.size();
	}


	for (int i = 0; i < numHost; i++) {
		for (int j = 0; j < hostCore[i]; j++) {
			int numTask = rand() % 10 + 1;
			hostCoreTask[i][j].resize(numTask);
			for (int k = 0; k < numTask; k++) {
				int jid = rand() % numJob;
				int bid = rand() % jobBlock[jid];
				int endTime = hostCoreFinishTime[i][j] + rand() % 100 + 1;
				hostCoreTask[i][j][k] = make_tuple(jid, bid, hostCoreFinishTime[i][j], endTime);
				hostCoreFinishTime[i][j] = endTime;
			}
		}
	}
}

void ResourceScheduler::outputSolutionFromBlock() {
	cout << "\nTask2 Solution (Block Perspective) of Teaching Assistant:\n\n";
	for (int i = 0; i < numJob; i++) {
		double speed = g(jobCore[i]);
		cout << "Job" << i << " obtains " << jobCore[i] << " cores (speed=" << speed << ") and finishes at time " << jobFinishTime[i] << ": \n";
		for (int j = 0; j < jobBlock[i]; j++) {
			cout << "\tBlock" << j << ": H" << get<0>(runLoc[i][j]) << ", C" << get<1>(runLoc[i][j]) << ", R" << get<2>(runLoc[i][j]) << " (time=" << fixed << setprecision(2) << dataSize[i][j] / speed << ")" << " \n";
		}
		cout << "\n";
	}

	cout << "The maximum finish time: " << *max_element(jobFinishTime.begin(), jobFinishTime.end()) << "\n";
	cout << "The total response time: " << accumulate(jobFinishTime.begin(), jobFinishTime.end(), 0.0) << "\n\n";
}

void ResourceScheduler::outputSolutionFromCore() {
	cout << "\nTask2 Solution (Core Perspective) of Teaching Assistant:\n\n";
	double maxHostTime = 0, totalRunningTime = 0.0;
	for (int i = 0; i < numHost; i++) {
		double hostTime = *max_element(hostCoreFinishTime[i].begin(), hostCoreFinishTime[i].end());
		maxHostTime = max(hostTime, maxHostTime);
		totalRunningTime += accumulate(hostCoreFinishTime[i].begin(), hostCoreFinishTime[i].end(), 0.0);
		cout << "Host" << i << " finishes at time " << hostTime << ":\n\n";
		for (int j = 0; j < hostCore[i]; j++) {
			cout << "\tCore" << j << " has " << hostCoreTask[i][j].size() << " tasks and finishes at time " << hostCoreFinishTime[i][j] << ":\n";
			for (int k = 0; k < hostCoreTask[i][j].size(); k++) {
				cout << "\t\tJ" << setw(2) << setfill('0') << get<0>(hostCoreTask[i][j][k]) << ", B" << setw(2) << setfill('0') << get<1>(hostCoreTask[i][j][k]) << ", runTime " << fixed << setprecision(1) << setw(5) << setfill('0') << get<2>(hostCoreTask[i][j][k]) << " to " << fixed << setprecision(1) << setw(5) << setfill('0') << get<3>(hostCoreTask[i][j][k]) << "\n";
			}
			cout << "\n";
		}
		cout << "\n\n";
	}
	cout << "The maximum finish time of hosts: " << maxHostTime << "\n";
	cout << "The total efficacious running time: " << totalRunningTime << "\n";
	cout << "Utilization rate: " << totalRunningTime / accumulate(hostCore.begin(), hostCore.end(), 0.0) / maxHostTime << "\n\n";
}

void ResourceScheduler::visualization() {

}

double ResourceScheduler::g(int e) {
	return 1 - alpha * (e - 1);
}

void ResourceScheduler::firstJobSchedule() {
	vector<double> totalJobTime;
	totalJobTime.resize(numJob);
	for (int i = 0; i < numJob; i++) {
		totalJobTime[i] = 0;
		for (int j = 0; j < jobBlock[i]; j++) {
			totalJobTime[i] += blockTime[i][0][j];
		}
	}

	sort(totalJobTime.begin(), totalJobTime.end(), less<double>());


}

void ResourceScheduler::scheduleTwoStep() {
	priority_queue<pair<double, int>, vector<pair<double, int>>, less<pair<double, int> > > heapJob; //max-heap
	priority_queue<double, vector<double>, greater<double> > heapFinish; //min-heap
	for (int i = 0; i < hostCore[0]; i++) heapFinish.push(0.0);

	double finishLine = 0;
	for (int i = 0; i < numJob; i++) {
		finishLine += blockSch[i][0][0];
		heapJob.push(make_pair(blockSch[i][0][0], i));
	}

	finishLine /= double(hostCore[0]);
	double perfectTime = finishLine;

	int coreNum = 0;
	while (coreNum < hostCore[0] && (!heapJob.empty()))
	{
		priority_queue<pair<double, int>, vector<pair<double, int>>, less<pair<double, int> > > heapJobTemp;
		while (!heapJob.empty())
		{
			pair<double, int> jobi = heapJob.top();
			heapJob.pop();
			vector<double> blockFinish;
			vector<double> jobFinish;
			for (int i = 0; i <= coreNum; i++) blockFinish.push_back(blockSch[jobi.second][coreNum][i]);
			double maxFinish = 0;
			for (int i = 0; i <= coreNum; i++)
			{
				maxFinish = max(maxFinish, heapFinish.top());
				jobFinish.push_back(heapFinish.top());
				heapFinish.pop();
			}
			if (maxFinish + blockFinish[coreNum] <= finishLine)
			{
				for (int i = 0; i <= coreNum; i++)
				{
					jobFinish[i] = maxFinish + blockFinish[i];
					heapFinish.push(jobFinish[i]);
				}
			}
			else
			{
				for (int i = 0; i <= coreNum; i++) heapFinish.push(jobFinish[i]);
				heapJobTemp.push(jobi);
			}
		}
		heapJob = heapJobTemp;



		if (coreNum == hostCore[0] - 1 && (!heapJob.empty()))
		{
			//À©ÈÝ finishLine 
			coreNum = 0;
			double augLine = 0;
			int heapJobTempSize = heapJobTemp.size();
			while (!heapJobTemp.empty())
			{
				augLine += heapJobTemp.top().first;
				heapJobTemp.pop();
			}
			augLine /= double(heapJobTempSize);
			finishLine += augLine;
			//finishLine = augLine + heapFinish.top();
		}
		else
		{
			coreNum++;
		}
	}

	cout << "All cores finish time: " << endl;
	double infactTime = 0;
	while (!heapFinish.empty())
	{
		if (heapFinish.top() > infactTime) {
			infactTime = heapFinish.top();
		}
		cout << heapFinish.top() << ' ';
		heapFinish.pop();
	}
	cout << endl;

	cout << "infactTime / perfectTime = " << infactTime / perfectTime << endl;
}

void ResourceScheduler::scheduleTwoStep2() {
	priority_queue<pair<double, int>, vector<pair<double, int>>, less<pair<double, int> > > heapJob; //max-heap
	priority_queue<double, vector<double>, greater<double> > heapFinish; //min-heap
	for (int i = 0; i < hostCore[0]; i++) heapFinish.push(0.0);

	double finishLine = 0;
	for (int i = 0; i < numJob; i++) {
		finishLine += blockSch[i][0][0];
		heapJob.push(make_pair(blockSch[i][0][0], i));
	}

	finishLine /= double(hostCore[0]);
	double perfectTime = finishLine;

	int coreNum = 0;
	while ((!heapJob.empty()))
	{
		priority_queue<pair<double, int>, vector<pair<double, int>>, less<pair<double, int> > > heapJobTemp;
		while (!heapJob.empty())
		{
			pair<double, int> jobi = heapJob.top();
			heapJob.pop();
			vector<double> blockFinish;
			vector<double> jobFinish;
			for (int i = 0; i <= coreNum; i++) blockFinish.push_back(blockSch[jobi.second][coreNum][i]);
			double maxFinish = 0;
			for (int i = 0; i <= coreNum; i++)
			{
				maxFinish = max(maxFinish, heapFinish.top());
				jobFinish.push_back(heapFinish.top());
				heapFinish.pop();
			}
			if (maxFinish + blockFinish[coreNum] <= finishLine)
			{
				for (int i = 0; i <= coreNum; i++)
				{
					jobFinish[i] = maxFinish + blockFinish[i];
					heapFinish.push(jobFinish[i]);
				}
			}
			else
			{
				for (int i = 0; i <= coreNum; i++) heapFinish.push(jobFinish[i]);
				heapJobTemp.push(jobi);
			}
		}
		heapJob = heapJobTemp;



		if ((!heapJob.empty()))
		{
			//À©ÈÝ finishLine 
			double augLine = 0;
			int heapJobTempSize = heapJobTemp.size();
			while (!heapJobTemp.empty())
			{
				augLine += heapJobTemp.top().first;
				heapJobTemp.pop();
			}
			augLine /= double(heapJobTempSize);
			finishLine += augLine;
			//finishLine = augLine + heapFinish.top();
		}
		else
		{
			;
		}
	}

	//cout << "All cores finish time: " << endl;
	double infactTime = 0;
	while (!heapFinish.empty())
	{
		if (heapFinish.top() > infactTime) {
			infactTime = heapFinish.top();
		}
		cout << heapFinish.top() << ' ';
		heapFinish.pop();
	}
	cout << endl;
	cout << "infactTime / perfectTime = " << infactTime / perfectTime << endl;
}